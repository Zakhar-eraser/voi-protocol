#include <iostream>
#include <chrono> 
#include <thread>

#include "mavlink/common/mavlink.h"
#include "mavlink/ardupilotmega/ardupilotmega.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#if (defined __QNX__) | (defined __QNXNTO__)
/* QNX specific headers */
#include <unix.h>
#else
/* Linux / MacOS POSIX timer headers */
#include <sys/time.h>
#include <time.h>
#include <arpa/inet.h>
#include <stdbool.h> /* required for the definition of bool in C99 */
#endif

#define BUFFER_LENGTH 2041

// struct Test
// {
//     int freq : 32;

// };


struct Data 
{
    uint8_t command;
    uint32_t timeBoot;
    uint8_t mavType;
    uint8_t mavAutopilot;
    uint8_t mavVersion;
};

struct GPS
{
    int8_t command;
    int32_t lat;
    int32_t lon;
    int32_t alt;
    int32_t relativeAlt;
};

union Packet
{
    Data data;
    GPS gps;
};

extern int qwert;

extern Packet testPacket;

class GetSock
{

public:

	bool debug;
    struct sockaddr_in locAddr;
    struct sockaddr_in gcAddr;
    struct sockaddr_in postAddr;
	uint8_t buf[BUFFER_LENGTH];
	socklen_t fromlen = sizeof(gcAddr);
	int bytes_sent;
	int status;
    int sock;
    // Packet updatePacket;

    GetSock();
    GetSock(const char *host_, int port_);
    GetSock(const char *gchost_ , int gcport_, const char *posthost_ , int postport_);

    static int a(int x);
    int b(int x);
    int c();
    
    int startListening(GetSock sock, uint32_t hz);
    int startMessaging(Packet packet, GetSock sock, uint32_t hz);
    int updateMessaging(Packet packet);
    int sendMessage();
	int send_mav_message(const mavlink_command_long_t &message);
	int send_common_message(const mavlink_message_t &message);
    

private:

	int  fd;
	mavlink_status_t lastStatus;
    
    
    void startListening_();
    static void threadListening_(GetSock sock,uint32_t hz);
    static void threadMessaging_(Packet packet, GetSock sock, uint32_t hz);

    void sock_bind();

};