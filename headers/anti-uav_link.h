#ifndef _HEADERS_ANTI_UAV_LINK_H_
#define _HEADERS_ANTI_UAV_LINK_H_

#include <stdint.h>

struct module_status {
    uint32_t time1;
    uint32_t time2;
    uint32_t status : 3;
    uint32_t work : 3;
    uint32_t isRGDV : 1;
    uint32_t isRAF : 1;
    uint32_t isLocal : 1;
    uint32_t isImit : 1;
    uint32_t hasTP : 1;
    uint32_t isTP : 1;
    uint32_t reserved1 : 4;
    uint32_t mode : 16;
    uint32_t reserved2;
};

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
    uint32_t idxModule : 8;
    uint32_t errorConnect : 8;
} reg_confirm;

typedef struct {
    uint32_t timeRequest1;
    uint32_t timeRequest2;
} time_request;

struct time_response{
    uint32_t timeRequest1;
    uint32_t timeRequest2;
    uint32_t timeAnswer1;
    uint32_t timeAnswer2;
};

typedef struct {
    uint32_t serial;
    float maxRange;
    float maxV;
    float maxHeight;
    uint32_t isGround : 1;
    uint32_t isAerial : 2;
} bla_abil; //0xA21

typedef struct {
    uint32_t serial;
    uint32_t flagErr : 16;
    uint32_t typeBLA : 8;
    uint32_t typeBCH : 8;
    uint32_t stateBLA : 8;
    uint32_t stateBCH : 8;
    uint32_t portVideo : 16;
    float FreqLinkHz;
    float SNR_dB;
    uint32_t typeCoordCeil : 1;
    uint32_t typeCoordBLA : 2;
    uint32_t typeSpeed : 2;
    float Lat_XCeil;
    float Lon_YCeil;
    float H_ZCeil;
    float Lat_XBLA;
    float Lon_YBLA;
    float H_ZBLA;
    uint32_t timeCoordBLA1;
    uint32_t timeCoordBLA2;
    float V_Vx;
    float Course_Vy;
    float Vh_Vz;
} bla_state; //0xA23

typedef struct {
    uint32_t serial;
    uint8_t cmdBLA;
    uint8_t cmdBCH;
    float latitudeDegCeil;
    float longitudeDegCeil;
    float heightCeil;
} control_cmd;

typedef struct {
    uint32_t serial;
    uint32_t cmdBLA : 8;
    uint32_t cmdBCH : 8;
    uint32_t typeCoord : 1;
    uint32_t typeSpeed : 2;
    uint32_t typeAccel : 2;
    uint32_t timeCeil1;
    uint32_t timeCeil2;
    float latitudeDegCeil;
    float longitudeDegCeil;
    float heightCeil;
    float V_Vx;
    float Course_Vy;
    float Vh_yZ;
    float aX;
    float aY;
    float aZ;
} ext_control_cmd;

typedef struct {
    uint32_t serial;
    uint32_t typeCoordBLA : 1;
    uint32_t typeSpeedBLA : 2;
    uint32_t typeAccelBLA : 2;
    uint32_t timeCeil1;
    uint32_t timeCeil2;
    float latitudeDegBLA;
    float longitudeDegBLA;
    float heighBLA;
    float V_VxBLA;
    float Cource_VyBLA;
    float Vh_yZBLA;
    float aXBLA;
    float aYBLA;
    float aZBLA;
} coord_cor_cmd;

typedef struct {
    uint32_t serial;
    uint32_t cmdBLA : 8;
    uint32_t cmdBCH : 8;
    uint32_t typeCoordBLA : 1;
    uint32_t timeCeil1;
    uint32_t timeCeil2;
    float delX_Az;
    float delY_Eps;
    float delZ;
} mismatch_cmd;

#endif // _HEADERS_ANTI_UAV_LINK_H_
