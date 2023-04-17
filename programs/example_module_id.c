#include "anti-uav_link.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#define ADDRESS "::"
#define PORT 17001
#define MODULE_ID 0x1
#define SERIAL_NUMBER 2345

int main() {
    int out = 1;
    int sock = socket(AF_INET6, SOCK_STREAM, 0);
    if (sock != -1) {
        struct sockaddr_in6 addr = {0};
        if (inet_pton(AF_INET6, ADDRESS, &(addr.sin6_addr)) == 1) {
            addr.sin6_family = AF_INET6;
            addr.sin6_port = htons(PORT);
            if (!connect(sock, (struct sockaddr *)&addr, sizeof(addr))) {
                set_module_info(MODULE_ID, 2, MODULE_ID, 0);
                set_module_idx(0xFF);
                register_request req = {.idManuf = MODULE_ID, .isAsku = 0,
                    .isInfo = 0, .serialNum = SERIAL_NUMBER, .versHardMin = 0,
                    .versHardMaj = 0, .versProgMaj = 1, .versProgMin = 0};
                reg_req_pack pack = pack_register_request(&req);
                if (send(sock, pack.words, sizeof(pack), 0) > 0) {
                    reg_conf_pack resp_pack;
                    if (recv(sock, resp_pack.words, sizeof(resp_pack), 0)) {
                        out--;
                        register_confirm resp = unpack_reg_conf(&resp_pack);
                        printf("Response:\nid: %i\nerrno: %i", resp.idxModule,resp.errorConnect);
                    } else {
                        printf("Receiving failed: %i", errno);
                    }
                } else {
                    printf ("Transmitting failed: %i", errno);
                }
            } else {
                printf("Can`t connect: %i", errno);
            }
        } else {
            printf("Invalid IPv6 address format: %i", errno);
        }
        close(sock);
    } else {
        printf("Socket creation failed: %i", errno);
    }
    return out;
}