#ifndef HEADERS_ANTI_UAV_TCP_CONNECTOR_H_
#define HEADERS_ANTI_UAV_TCP_CONNECTOR_H_
#include "anti-uav_link.h"

void set_header_info(uint32_t sender, uint8_t yMajor,
                     uint8_t yMinor, uint8_t isAsku);
void set_time_response_callback(void (*callback)(header *, time_response *));
void set_control_cmd_callback(void (*callback)(header *, control_cmd *));
void set_ext_control_cmd_callback(void (*callback)(header *, ext_control_cmd *));
void set_coord_cor_cmd_callback(void (*callback)(header *, coord_cor_cmd *));
void set_mismatch_cmd_callback(void (*callback)(header *, mismatch_cmd *));
void set_unknown_message_handler(void (*callback)(header *, uint32_t *));

int send_nsu_abilities(uint8_t maxNumBLA, bla_abil *abil);
int voi_register(char *ipv6_address, int port, reg_request *req);
int send_bla_state(bla_state *state);

void voi_start_listen();
void voi_stop_listen();
void wait_lost_connection();

header get_common_header();

void close_voi_connection();

#endif // HEADERS_ANTI_UAV_TCP_CONNECTOR_H_
