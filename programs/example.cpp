#include "anti-uav_tcp_connector.h"
#include "get_data.h"
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>

#define ADDRESS "192.168.0.12"
#define PORT 17001
#define MODULE_ID 0x424c41
#define MANUFACTURER_ID 0x1A
#define SERIAL_NUMBER 2345
#define PROTOCOL_VERSION_MAJOR 2
#define PROTOCOL_VERSION_MINOR 3
#define FREQ 2

static GetSock s("127.0.0.1", 14552, "127.0.0.1", 14553);
static Packet coord_pack = {0};
static bla_state state = {.serial = 12345, .typeBLA = 1,
                       .typeBCH = 2, .stateBLA = 1,
                       .stateBCH = 1, .FreqLinkHz = 10,
                       .SNR_dB = 100, .typeCoordCeil = 1,
                       .typeCoordBLA = 1, .typeSpeed = 2};
static module_status status = {.status = 1, .work = 1, .isRAF = 1, .isImit = 1};

void unexpected_message_callback(header *hdr, void *pack) {
    (void)pack;
    fprintf(stderr, "pack 0x%X captured\n", hdr->typePack);
}

void ext_control_command_callback(header *hdr, ext_control_cmd *cmd) {
    coord_pack.gps.command = cmd->cmdBLA;
    coord_pack.gps.lat = cmd->latitudeDegCeil;
    coord_pack.gps.lon = cmd->longitudeDegCeil;
    coord_pack.gps.alt = cmd->heightCeil;
    fprintf(stderr, "pack 0x%X captured\nX: %f\nY: %f\nZ: %f\ncommand: %i\n", hdr->idxPack,
        cmd->latitudeDegCeil, cmd->longitudeDegCeil, cmd->heightCeil, cmd->cmdBLA);
    s.updateMessaging(coord_pack);
}

void coords_timer_callback(int signum) {
    (void)signum;
    State st = s.getCurrentState();
    state.Lat_XBLA = st.x;
    state.Lon_YBLA = st.y;
    state.H_ZBLA = st.z;
    state.V_Vx = st.vx;
    state.Course_Vy = st.vy;
    state.Vh_Vz = st.vz;
    send_bla_state(&state);
}

void close_connection(int signum) {
    (void)signum;
    voi_stop_listen();
}

int main() {
    FILE *file = fopen("../json/scheme.json", "rb");
    fseek(file, 0, SEEK_END);
    unsigned long size = ftell(file) + 1;
    fseek(file, 0, SEEK_SET);
    uint8_t *json = (uint8_t *)malloc(size);
    fread(json, size - 1, 1, file);
    fclose(file);
    json[size-1] = 0;

    coord_pack.gps.command = 0;
    coord_pack.gps.lat =11;
    coord_pack.gps.lon = 5;
    coord_pack.gps.alt = 12;
    s.startListening(s, 10);
    s.startMessaging(coord_pack, s, 100);

    signal(EINTR, close_connection);
    struct itimerval new_timer = {0}, old_timer = {0};
    new_timer.it_value.tv_sec = 2;
    new_timer.it_interval.tv_usec = 1000000 / FREQ;

    set_header_info(MODULE_ID, PROTOCOL_VERSION_MAJOR, PROTOCOL_VERSION_MINOR, 0);
    set_module_status(status);
    reg_request request = {.idManuf = MANUFACTURER_ID, .serialNum = SERIAL_NUMBER,
                            .versHardMaj = 1, .versHardMin = 0,
                            .versProgMaj = 1, .isInfo = 0,
                            .versProgMin = 0 ,.isAsku = 0};
    bla_abil abil = {.serial = 12345, .maxRange = 100,
                     .maxV = 20, .maxHeight = 100,
                     .isGround = 0, .isAerial = 1};
    module_geopos geopos = {.typeData = 2, .isValid = 1};
    set_ext_control_cmd_callback((void (*)(header *, ext_control_cmd *))unexpected_message_callback);
    set_control_cmd_callback((void (*)(header *, control_cmd *))unexpected_message_callback);
    set_coord_cor_cmd_callback((void (*)(header *, coord_cor_cmd *))unexpected_message_callback);
    set_mismatch_cmd_callback((void (*)(header *, mismatch_cmd *))unexpected_message_callback);
    set_unknown_message_handler((void (*)(header *, uint32_t *))unexpected_message_callback);
    set_ext_control_cmd_callback(ext_control_command_callback);
    if (voi_register((char *)ADDRESS, PORT, &request)) {
        printf("Error %i", errno);
    } else {
        send_module_scheme(json, size);
        send_module_geopos(&geopos);
        send_nsu_abilities(1, &abil);
        voi_start_listen();
        setitimer(ITIMER_REAL, &new_timer, &old_timer);
        signal(SIGALRM, coords_timer_callback);
        wait_lost_connection();
    }
    close_voi_connection();
    free(json);
    return errno;
}