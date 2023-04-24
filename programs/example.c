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

void packet_callback(header *hdr, void *pack) {
    (void)pack;
    fprintf(stderr, "pack 0x%X captured\n", hdr->typePack);
}

int main() {
    set_header_info(MODULE_ID, PROTOCOL_VERSION_MAJOR, PROTOCOL_VERSION_MINOR, 0);
    reg_request request = {.idManuf = MANUFACTURER_ID, .isAsku = 0,
        .isInfo = 0, .serialNum = SERIAL_NUMBER,
        .versHardMaj = 1, .versHardMin = 0,
        .versProgMaj = 1, .versProgMin = 0};
    bla_abil abil = {.serial = 12345, .isAerial = 1, .maxHeight = 50, .maxRange = 1, .maxV = 20};
    bla_state state = {.serial = 12345, .typeBLA = 1, .typeBCH = 2, .stateBLA = 1, .stateBCH = 1, .FreqLinkHz = 1, .SNR_dB = 5};
    set_time_response_callback((void (*)(header *, time_response *))packet_callback);
    set_ext_control_cmd_callback((void (*)(header *, ext_control_cmd *))packet_callback);
    set_coord_cor_cmd_callback((void (*)(header *, coord_cor_cmd *))packet_callback);
    set_mismatch_cmd_callback((void (*)(header *, mismatch_cmd *))packet_callback);
    set_control_cmd_callback((void (*)(header *, ext_control_cmd *))packet_callback);
    if (voi_register(ADDRESS, PORT, &request)) {
        printf("Error %i", errno);
    } else {
        send_nsu_abilities(1, &abil);
        send_bla_state(&state);
        voi_start_listen();
        wait_lost_connection();
    }
    close_voi_connection();
    return errno;
}