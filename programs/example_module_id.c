#include "anti-uav_tcp_connector.h"
#include <stdio.h>
#include <errno.h>

#define ADDRESS "::"
#define PORT 17001
#define MODULE_ID 0x424c41
#define MANUFACTURER_ID 0x1
#define SERIAL_NUMBER 2345
#define PROTOCOL_VERSION_MAJOR 2
#define PROTOCOL_VERSION_MINOR 3

int main() {
    set_header_info(MODULE_ID, PROTOCOL_VERSION_MAJOR, PROTOCOL_VERSION_MINOR, 0);
    reg_request request = {.idManuf = MANUFACTURER_ID, .isAsku = 0,
        .isInfo = 0, .serialNum = SERIAL_NUMBER,
        .versHardMaj = 1, .versHardMin = 0,
        .versProgMaj = 1, .versProgMin = 0};
    if (!voi_register(ADDRESS, PORT, &request)) {
        printf("Error %i", errno);
    }
    return errno;
}