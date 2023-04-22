#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include "anti-uav_tcp_connector.h"
//#define JSMN_HEADER
//#include "jsmn/jsmn.h"

int send_voi_message(header *hdr, void *pack);
void set_check_sum(header *header);
void *sender_thread(void *flag);
void *receiver_thread(void *flag);
time_request get_time_request_msg(header *header);

static header common_header = {0};
static int sockfd = -1;
static uint16_t idxPack = 0;
static pthread_t sender_id, receiver_id;
static uint8_t stop;

void voi_start_listen() {
    stop = 0;
    pthread_create(&sender_id, NULL, sender_thread, &stop);
    pthread_create(&receiver_id, NULL, receiver_thread, &stop);
}

void *sender_thread(void *flag) {
    uint8_t *stop_flag = (uint8_t *)flag;
    while (!(*stop_flag)) {
        sleep(5);
        time_request request = get_time_request_msg(&common_header);
        *stop_flag = !send_voi_message(&common_header, &request);
    }
    pthread_exit(NULL);
}

void *receiver_thread(void *flag) {
    uint8_t *stop_flag = (uint8_t *)flag;
    while (!(*stop_flag)) {
        sleep(1);
    }
    pthread_exit(NULL);
}

time_request get_time_request_msg(header *header) {
    header->sizeData = 8;
    header->typePack = 0x3;
    time_t t = time(NULL);
    time_request request = {.timeRequest1 = (uint32_t)t,
        .timeRequest2 = (uint32_t)(t >> 32)};
    return request;
}

void voi_stop_listen() {
    stop = 1;
}

void wait_lost_connection() {
    pthread_join(sender_id, NULL);
    pthread_join(receiver_id, NULL);
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
                common_header.idxPack = idxPack;
                common_header.sizeData = sizeof(reg_request);
                if (send_voi_message(&common_header, req)) {
                    header hdr;
                    reg_confirm conf;
                    struct iovec msg_iov[2] = {{.iov_base = &hdr, .iov_len = sizeof(hdr)},
                        {.iov_base = &conf, .iov_len = sizeof(conf)}};
                    struct msghdr msg = {.msg_iov = msg_iov, .msg_iovlen = 2};
                    if (recvmsg(sockfd, &msg, 0) == 20) {
                        common_header.idxModule = conf.idxModule;
                    }
                }
            }
        }
    }
    return errno;
}

int send_voi_message(header *hdr, void *pack) {
    int out = 0;
    set_check_sum(hdr);
    struct iovec msg_iov[2] = {{.iov_base = hdr, .iov_len = 16},
        {.iov_base = pack, .iov_len = hdr->sizeData}};
    struct msghdr msg = {.msg_iov = msg_iov, .msg_iovlen = 2};
    if (sendmsg(sockfd, &msg, 0) == 16 + hdr->sizeData) {
        idxPack++;
        out = 1;
    }
    return out;
}

void set_check_sum(header *header) {
    header->checkSum = header->idxModule + header->yMajor +
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
