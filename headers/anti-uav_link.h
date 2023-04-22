#ifndef _HEADERS_ANTI_UAV_LINK_H_
#define _HEADERS_ANTI_UAV_LINK_H_

#include <stdint.h>

typedef struct {
    uint32_t sender : 24;
    uint32_t idxModule : 8;
    uint32_t yMajor : 8;
    uint32_t yMinor : 8;
    uint32_t idxPack : 16;
    uint32_t sizeData : 31;
    uint32_t isAsku : 1;
    uint32_t typePack : 16;
    uint32_t checkSum : 16;
} header;

typedef struct {
    uint32_t idManuf : 8;
    uint32_t serialNum : 24;
    uint32_t versHardMaj : 8;
    uint32_t versHardMin : 8;
    uint32_t versProgMaj : 7;
    uint32_t isInfo : 1;
    uint32_t versProgMin : 7;
    uint32_t isAsku : 1;
} reg_request;

typedef struct {
    uint32_t idxModule: 8;
    uint32_t errorConnect: 8;
} reg_confirm;

typedef struct {
    uint32_t timeRequest1;
    uint32_t timeRequest2;
} time_request;

typedef struct {
    uint32_t timeRequest1;
    uint32_t timeRequest2;
    uint32_t timeAnswer1;
    uint32_t timeAnswer2;
} time_response;

#endif  // _HEADERS_ANTI_UAV_LINK_H_
