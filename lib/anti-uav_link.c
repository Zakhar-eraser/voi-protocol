//#define JSMN_HEADER
//#include "jsmn/jsmn.h"
#include "anti-uav_link.h"
#include <string.h>

static header common_header = {0};

uint16_t int32_byte_sum(uint32_t val);

register_confirm unpack_reg_conf(reg_conf_pack *pack) {
    register_confirm conf = {.idxModule = (uint8_t)(pack->words[4]),
        .errorConnect = (uint8_t)(pack->words[5] >> 8)};
    return conf;
}

reg_req_pack pack_register_request(register_request *reg_req) {
    common_header.typePack = 0x1;
    common_header.sizeData = sizeof(reg_req_pack) - sizeof(hdr_pack);
    reg_req_pack pack;
    memcpy(pack.words, pack_header(&common_header).words, 16);
    pack.words[4] = reg_req->serialNum << 8;
    pack.words[4] |= reg_req->idManuf & UINT8_MAX;

    pack.words[5] = reg_req->versHardMaj & UINT8_MAX;
    pack.words[5] |= (reg_req->versHardMin & UINT8_MAX) << 8;
    pack.words[5] |= (reg_req->versProgMaj & (UINT8_MAX >> 1)) << 16;
    pack.words[5] |= (reg_req->isInfo & 1) << 23;
    pack.words[5] |= (reg_req->versProgMin & (UINT8_MAX >> 1)) << 24;
    pack.words[5] |= (reg_req->isAsku & 1) << 31;
    return pack;
}

hdr_pack pack_header(header *hdr) {
    hdr_pack pack;
    pack.words[0] = hdr->sender & (UINT32_MAX >> 8);
    pack.words[0] |= hdr->idxModule << 24;

    pack.words[1] = hdr->yMajor & UINT8_MAX;
    pack.words[1] |= (hdr->yMinor & UINT8_MAX) << 8;
    pack.words[1] |= hdr->idxPack << 16;

    pack.words[2] = hdr->sizeData & (UINT32_MAX >> 1);
    pack.words[2] = hdr->isAsku << 31;

    pack.words[3] = hdr->typePack & UINT16_MAX;
    uint32_t checksum = int32_byte_sum(pack.words[0]) +
        int32_byte_sum(pack.words[1]) + int32_byte_sum(pack.words[2]) +
        (uint8_t)(pack.words[3]) + (uint8_t)(pack.words[3] >> 8);
    pack.words[3] |= checksum << 16;
    return pack;
}

uint16_t int32_byte_sum(uint32_t val) {
    return (uint8_t)val + (uint8_t)(val >> 8) +
        (uint8_t)(val >> 16) + (uint8_t)(val >> 24);
}

void set_module_info(
    uint32_t sender,
    uint32_t yMajor,
    uint32_t yMinor,
    uint32_t isAsku) {
    common_header.sender = sender;
    common_header.yMajor = yMajor;
    common_header.yMinor = yMinor;
    common_header.isAsku = isAsku;
}

void set_module_idx(uint32_t idxModule) {
    common_header.idxModule = idxModule;
}
