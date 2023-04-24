#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "anti-uav_tcp_connector.h"
//#define JSMN_HEADER
//#include "jsmn/jsmn.h"

int send_voi_message(struct msghdr *msg);
uint16_t get_check_sum(header *header);
void *ping_thread(void *flag);
void *receiver_thread(void *flag);
int send_time_request();

void ignore_message(header *hdr, void *pack);
static void (*time_response_handler)(header *, time_response *) =
    (void (*)(header *, time_response *))ignore_message;
static void (*control_cmd_handler)(header *, ext_control_cmd *) =
    (void (*)(header *, ext_control_cmd *))ignore_message;
static void (*ext_control_cmd_handler)(header *, ext_control_cmd *) =
    (void (*)(header *, ext_control_cmd *))ignore_message;
static void (*coord_cor_cmd_handler)(header *, coord_cor_cmd *) =
    (void (*)(header *, coord_cor_cmd *))ignore_message;
static void (*mismatch_cmd_handler)(header *, mismatch_cmd *) =
    (void (*)(header *, mismatch_cmd *))ignore_message;
static void (*unknown_message_handler)(header *, uint32_t *) =
    (void (*)(header *, uint32_t *))ignore_message;

static header common_header = {0};
static int sockfd = -1;
static pthread_t ping_id, receiver_id;
static uint8_t stop;
static pthread_mutex_t send_mutex;

int send_bla_state(bla_state *state) {
    header hdr = common_header;
    hdr.typePack = 0xA23;
    hdr.sizeData = sizeof(bla_state);
    *(uint64_t *)&(state->timeCoordBLA1) = time(NULL);
    struct iovec msg_iov[2] = {{.iov_base = &hdr, .iov_len = 16},
        {.iov_base = state, .iov_len = sizeof(bla_state)}};
    struct msghdr msg = {.msg_iov = msg_iov, .msg_iovlen = 2};
    return send_voi_message(&msg);
}

int send_nsu_abilities(uint8_t maxNumBLA, bla_abil *abil) {
    header hdr = common_header;
    hdr.typePack = 0xA21;
    hdr.sizeData = 4 + sizeof(bla_abil) * maxNumBLA;
    uint32_t tmp = maxNumBLA;
    struct iovec msg_iov[3] = {{.iov_base = &hdr, .iov_len = 16},
        {.iov_base = &tmp, .iov_len = 4},
        {.iov_base = abil, .iov_len = sizeof(bla_abil) * maxNumBLA}};
    struct msghdr msg = {.msg_iov = msg_iov, .msg_iovlen = 3};
    return send_voi_message(&msg);
}

void voi_start_listen() {
    stop = 0;
    pthread_mutex_init(&send_mutex, NULL);
    pthread_create(&ping_id, NULL, ping_thread, &stop);
    pthread_create(&receiver_id, NULL, receiver_thread, &stop);
}

void *ping_thread(void *flag) {
    uint8_t *stop_flag = (uint8_t *)flag;
    while (!(*stop_flag)) {
        sleep(5);
        *stop_flag = !send_time_request();
    }
    pthread_exit(NULL);
}

void *receiver_thread(void *flag) {
    uint8_t *stop_flag = (uint8_t *)flag;
    while(!(*stop_flag)) {
        header hdr;
        if (recv(sockfd, &hdr, sizeof(hdr), 0) &&
            hdr.checkSum == get_check_sum(&hdr)) {
            switch (hdr.typePack) {
                case 0x4:
                    time_response resp;
                    if (recv(sockfd, &resp, hdr.sizeData, 0))
                        time_response_handler(&hdr, &resp);
                    break;
                case 0x5A1:
                    control_cmd control_msg;
                    if (recv(sockfd, &control_msg, hdr.sizeData, 0))
                        control_cmd_handler(&hdr, &control_msg);
                case 0x5A2:
                    ext_control_cmd ext_control_msg;
                    if (recv(sockfd, &ext_control_msg, hdr.sizeData, 0))
                        ext_control_cmd_handler(&hdr, &ext_control_msg);
                    break;
                case 0x5A3:
                    coord_cor_cmd coord_cor_msg;
                    if (recv(sockfd, &coord_cor_msg, hdr.sizeData, 0))
                        coord_cor_cmd_handler(&hdr, &coord_cor_msg);
                    break;
                case 0x5A4:
                    mismatch_cmd mismatch_msg;
                    if (recv(sockfd, &mismatch_msg, hdr.sizeData, 0))
                        mismatch_cmd_handler(&hdr, &mismatch_msg);
                    break;
                default:
                    uint32_t *buf = malloc(hdr.sizeData);
                    if (recv(sockfd, buf, hdr.sizeData, 0))
                        unknown_message_handler(&hdr, buf);
                    free(buf);
                    break;
            }
        }
    }
    pthread_exit(NULL);
}

int send_time_request() {
    header hdr = common_header;
    hdr.sizeData = 8;
    hdr.typePack = 0x3;
    time_request request;
    *(uint64_t *)&request = time(NULL);
    struct iovec msg_iov[2] = {{.iov_base = &hdr, .iov_len = 16},
        {.iov_base = &request, .iov_len = hdr.sizeData}};
    struct msghdr msg = {.msg_iov = msg_iov, .msg_iovlen = 2};
    return send_voi_message(&msg);
}

void voi_stop_listen() {
    stop = 1;
}

void wait_lost_connection() {
    pthread_join(ping_id, NULL);
    pthread_join(receiver_id, NULL);
    pthread_mutex_destroy(&send_mutex);
}

int voi_register(char *ipv6_address, int port, reg_request *req) {
    if ((sockfd = socket(AF_INET6, SOCK_STREAM, 0)) != -1) {
        struct sockaddr_in6 addr = {0};
        if (inet_pton(AF_INET6, ipv6_address, &(addr.sin6_addr)) == 1) {
            addr.sin6_family = AF_INET6;
            addr.sin6_port = htons(port);
            if (!connect(sockfd, (struct sockaddr *)&addr, sizeof(addr))) {
                common_header.idxModule = 0xFF;
                common_header.typePack = 0x1;
                common_header.sizeData = sizeof(reg_request);
                struct iovec msg_iov[2] = {{.iov_base = &common_header, .iov_len = 16},
                    {.iov_base = req, .iov_len = sizeof(reg_request)}};
                struct msghdr msg = {.msg_iov = msg_iov, .msg_iovlen = 2};
                if (send_voi_message(&msg)) {
                    header hdr;
                    reg_confirm conf;
                    struct iovec rmsg_iov[2] = {{.iov_base = &hdr, .iov_len = sizeof(hdr)},
                        {.iov_base = &conf, .iov_len = sizeof(conf)}};
                    struct msghdr rmsg = {.msg_iov = rmsg_iov, .msg_iovlen = 2};
                    if (recvmsg(sockfd, &rmsg, 0)) {
                        common_header.idxModule = conf.idxModule;
                    }
                }
            }
        }
    }
    return errno;
}

int send_voi_message(struct msghdr *msg) {
    int out = 0;
    header *hdr = msg->msg_iov[0].iov_base;
    pthread_mutex_lock(&send_mutex);
        hdr->idxPack = common_header.idxPack;
        hdr->checkSum = get_check_sum(hdr);
        if (sendmsg(sockfd, msg, 0) == 16 + hdr->sizeData) {
            common_header.idxPack++;
            out = 1;
        }
    pthread_mutex_unlock(&send_mutex);
    return out;
}

uint16_t get_check_sum(header *header) {
    return header->idxModule + header->yMajor +
        header->yMinor + header->isAsku +
        (uint8_t)(header->idxPack) + (uint8_t)(header->idxPack >> 8) +
        (uint8_t)(header->typePack) + (uint8_t)(header->typePack >> 8) +
        (uint8_t)(header->sender) + (uint8_t)(header->sender >> 8) +
        (uint8_t)(header->sender >> 16) + (uint8_t)(header->sizeData) +
        (uint8_t)(header->sizeData >> 8) + (uint8_t)(header->sizeData >> 16) +
        (uint8_t)(header->sizeData >> 24);
}

void close_voi_connection() {
    close(sockfd);
}

void set_header_info(uint32_t sender, uint8_t yMajor,
        uint8_t yMinor, uint8_t isAsku) {
    common_header.sender = sender;
    common_header.yMajor = yMajor;
    common_header.yMinor = yMinor;
    common_header.isAsku = isAsku;
}

header get_common_header() {
    return common_header;
}

void ignore_message(header *hdr, void *pack) { (void)hdr; (void)pack; }

void set_time_response_callback(void (*callback)(header *, time_response *)) {
    time_response_handler = callback;
}

void set_control_cmd_callback(void (*callback)(header *, control_cmd *)) {
    control_cmd_handler = callback;
}

void set_ext_control_cmd_callback(void (*callback)(header *, ext_control_cmd *)) {
    ext_control_cmd_handler = callback;
}

void set_coord_cor_cmd_callback(void (*callback)(header *, coord_cor_cmd *)) {
    coord_cor_cmd_handler = callback;
}

void set_mismatch_cmd_callback(void (*callback)(header *, mismatch_cmd *)) {
    mismatch_cmd_handler = callback;
}

void set_unknown_message_handler(void (*callback)(header *, uint32_t *)) {
    unknown_message_handler = callback;
}
