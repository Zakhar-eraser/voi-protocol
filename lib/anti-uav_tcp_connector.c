#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include "anti-uav_tcp_connector.h"
//#define JSMN_HEADER
//#include "jsmn/jsmn.h"

int send_voi_message(void *pack);
void set_check_sum(header *header);

static header common_header = {0};
static int sockfd = -1;
static uint16_t idxPack = 0;

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
                if (send_voi_message(req)) {
                    header hdr;
                    reg_confirm conf;
                    struct iovec msg_iov[2] = {{.iov_base = &hdr, .iov_len = sizeof(hdr)},
                        {.iov_base = &conf, .iov_len = sizeof(conf)}};
                    struct msghdr msg = {.msg_iov = msg_iov, .msg_iovlen = 2};
                    if (recvmsg(sockfd, &msg, 0) == 24) {
                        common_header.idxModule = conf.idxModule;
                    }
                }
            }
        }
    }
    return errno;
}

int send_voi_message(void *pack) {
    int out = 0;
    set_check_sum(&common_header);
    struct iovec msg_iov[2] = {{.iov_base = &common_header, .iov_len = 16},
        {.iov_base = pack, .iov_len = common_header.sizeData}};
    struct msghdr msg = {.msg_iov = msg_iov, .msg_iovlen = 2};
    if (sendmsg(sockfd, &msg, 0) == 16 + common_header.sizeData) {
        idxPack++;
        out = 1;
    }
    return out;
}

void set_check_sum(header *header) {
    header->checkSum = header->idxModule + header->idxPack +
        header->isAsku + header->sender + header->sizeData +
        header->typePack + header->yMajor + header->yMinor;
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
