#ifndef HEADERS_ANTI_UAV_TCP_CONNECTOR_H_
#define HEADERS_ANTI_UAV_TCP_CONNECTOR_H_
#include "anti-uav_link.h"

void set_header_info(uint32_t sender, uint8_t yMajor,
        uint8_t yMinor, uint8_t isAsku);

int voi_register(char *ipv6_address, int port, reg_request *req);

void close_voi_connection();

#endif  // HEADERS_ANTI_UAV_TCP_CONNECTOR_H_
