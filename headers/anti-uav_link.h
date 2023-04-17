#ifndef _HEADERS_ANTI_UAV_LINK_H_
#define _HEADERS_ANTI_UAV_LINK_H_

#include <stdint.h>

typedef struct {
    uint32_t sender;
    uint32_t sizeData;
    uint32_t idxPack;
    uint32_t typePack;
    uint32_t idxModule;
    uint32_t yMajor;
    uint32_t yMinor;
    uint32_t isAsku;
} header;

typedef struct {
    uint32_t serialNum;
    uint32_t idManuf;
    uint32_t versHardMaj;
    uint32_t versHardMin;
    uint32_t versProgMaj;
    uint32_t isInfo;
    uint32_t versProgMin;
    uint32_t isAsku;
} register_request;

typedef struct {
    uint32_t idxModule;
    uint32_t errorConnect;
} register_confirm;

typedef struct {
    uint32_t words[4];
} hdr_pack;

typedef struct {
    uint32_t words[6];
} reg_req_pack;

typedef struct {
    uint32_t words[5];
} reg_conf_pack;

hdr_pack pack_header(header *hdr);
reg_req_pack pack_register_request(register_request *reg_req);

register_confirm unpack_reg_conf(reg_conf_pack *pack);

void set_module_info(uint32_t sender, uint32_t yMajor, uint32_t yMinor, uint32_t isAsku);
void set_module_idx(uint32_t idxModule);

#endif  // _HEADERS_ANTI_UAV_LINK_H_
