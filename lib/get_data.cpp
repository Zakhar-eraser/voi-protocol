#include "get_data.h"


int trigger = 0;
int triggerListening = 0;
int triggerMessaging = 0;

Packet updatePacket = {0};
State currentState = {0};
State outerState = {0};

std::mutex mtx;

class Timer
{
public:
  template<class T>
	void update(T& t, void (GetSock::*f) (), std::chrono::milliseconds Tend)
	{
		std::chrono::steady_clock::time_point tend = std::chrono::steady_clock::now() + Tend;
		while (triggerListening < 1)
		{
			if (std::chrono::steady_clock::now() > tend)
			{
				(t.*f)();
				tend = std::chrono::steady_clock::now() + Tend;
			}
		}
	}
	template<class T>
	void update2(T& t, int (GetSock::*f) (int), std::chrono::milliseconds Tend, int x)
	{
		std::chrono::steady_clock::time_point tend = std::chrono::steady_clock::now() + Tend;
		while (trigger < 1)
		{
			if (std::chrono::steady_clock::now() > tend)
			{
				(t.*f)(x);
				tend = std::chrono::steady_clock::now() + Tend;
			}
		}
	}
	template<class T>
	void updateMessaging(T& t, int (GetSock::*f) (), std::chrono::milliseconds Tend)
	{
		std::chrono::steady_clock::time_point tend = std::chrono::steady_clock::now() + Tend;
		while (triggerMessaging < 1)
		{
			if (std::chrono::steady_clock::now() > tend)
			{
				(t.*f)();
				tend = std::chrono::steady_clock::now() + Tend;
			}
		}
	}
};


GetSock::
GetSock()
{
    
}

GetSock::
GetSock(const char *host_, int port_)
{
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    memset(&locAddr, 0, sizeof(locAddr));
	locAddr.sin_family = AF_INET;
	// locAddr.sin_addr.s_addr = INADDR_ANY;
	locAddr.sin_addr.s_addr = inet_addr(host_);
	locAddr.sin_port = htons(port_);

    sock_bind();

    memset(&gcAddr, 0, sizeof(gcAddr));
	gcAddr.sin_family = AF_INET;
	gcAddr.sin_addr.s_addr = inet_addr(host_);
	gcAddr.sin_port = htons(port_);
}

GetSock::
GetSock(const char *gchost_ , int gcport_, const char *posthost_ , int postport_)
{
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    memset(&locAddr, 0, sizeof(locAddr));
	locAddr.sin_family = AF_INET;
	// locAddr.sin_addr.s_addr = INADDR_ANY;
	locAddr.sin_addr.s_addr = inet_addr(gchost_);
	locAddr.sin_port = htons(gcport_);

    sock_bind();

    memset(&gcAddr, 0, sizeof(gcAddr));
	gcAddr.sin_family = AF_INET;
	gcAddr.sin_addr.s_addr = inet_addr(gchost_);
	gcAddr.sin_port = htons(gcport_);

	memset(&postAddr, 0, sizeof(postAddr));
	postAddr.sin_family = AF_INET;
	postAddr.sin_addr.s_addr = inet_addr(posthost_);
	postAddr.sin_port = htons(postport_);


}

void
GetSock::
startListening_()
{
  	ssize_t recsize;
 	int i = 0;
	unsigned int temp = 0;

	memset(buf, 0, BUFFER_LENGTH);
	recsize = recvfrom(sock, (void *)buf, BUFFER_LENGTH, 0, (struct sockaddr *)&gcAddr, &fromlen);
	if (recsize > 0)
	{
		mavlink_message_t msg;
		mavlink_status_t status;
		mavlink_local_position_ned_t ned;
		mavlink_heartbeat_t heartbeat;
			for (i = 0; i < recsize; ++i)
			{
				temp = buf[i];
				if (mavlink_parse_char(MAVLINK_COMM_0, buf[i], &msg, &status))
				{
			switch(msg.msgid) {
				case MAVLINK_MSG_ID_LOCAL_POSITION_NED: // ID for GLOBAL_POSITION_INT
					{
					// printf("\nReceived packet: SYS: %d, COMP: %d, LEN: %d, MSG ID: %d\n", msg.sysid, msg.compid, msg.len, msg.msgid);
					mavlink_msg_local_position_ned_decode(&msg, &ned);
					// printf("data: %d", ned.x);
					mtx.lock();
					currentState.x = ned.x;
					currentState.y = ned.y;
					currentState.z = ned.z;
					currentState.vx = ned.vx;
					currentState.vy = ned.vy;
					currentState.vz = ned.vz;
					mtx.unlock();
			
				}
					break;
			// case MAVLINK_MSG_ID_HEARTBEAT: // ID for GLOBAL_POSITION_INT
			// 		{
			// 	// printf("\nReceived packet: SYS: %d, COMP: %d, LEN: %d, MSG ID: %d\n", msg.sysid, msg.compid, msg.len, msg.msgid);
			// 	// 	mavlink_msg_heartbeat_decode(&msg, &heartbeat);
			// 	// printf("data: %d", heartbeat.autopilot);
			
			// 	}
			// 		break;
				}
			}   
		}
		// printf("\n");
	}
	memset(buf, 0, BUFFER_LENGTH);
	// std::cout << "2" << std::endl;
	// sleep(0.01); 
}

void ha()
{
  	std::cout << "ha" << std::endl;
}

State
GetSock::
getCurrentState()
{
	outerState = currentState;
	return outerState;
}

int
GetSock::
sendMessage()
{
	mavlink_command_long_t armed = {0};
	armed.target_system = 1;
	armed.target_component = 0;
	armed.command = 414; 
	armed.confirmation = true;
	armed.param1 = updatePacket.gps.command;
	armed.param2 = updatePacket.gps.lat;
	armed.param3 = updatePacket.gps.lon;
	armed.param4 = updatePacket.gps.alt;
	armed.param5 = updatePacket.gps.command;

	mavlink_message_t msg;
	mavlink_msg_command_long_encode(1, 255, &msg, &armed);
	int len = mavlink_msg_to_send_buffer(buf, &msg);
	bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*)&postAddr, sizeof (struct sockaddr_in));	
}

int 
GetSock::
updateMessaging(Packet packet)
{
	updatePacket = packet;
}

int
GetSock::
startMessaging(Packet packet, GetSock sock, uint32_t hz)
{
	updatePacket = packet;
	std::thread threadMessaging(threadMessaging_, updatePacket, sock, hz);
	// std::thread threadStop(a, 2);
	// threadStop.detach();
	threadMessaging.detach();
}

void 
GetSock::
threadMessaging_(Packet packet, GetSock sock, uint32_t hz)
{
	Timer timer;
	std::chrono::milliseconds tend = std::chrono::milliseconds(hz);
	timer.updateMessaging(sock, &GetSock::sendMessage, tend);
}

int
GetSock::
startListening(GetSock sock, uint32_t hz)
{
	std::thread threadListening(threadListening_, sock, hz);
	// std::thread threadStop(a, 2);
	// threadStop.detach();
	threadListening.detach();
}

void 
GetSock::
threadListening_(GetSock sock, uint32_t hz)
{
	Timer timer;
	std::chrono::milliseconds tend = std::chrono::milliseconds(hz);
	timer.update(sock, &GetSock::startListening_, tend);
	// timer.update2(sock, &GetSock::b, tend, 2);
}

void
GetSock::
sock_bind()
{
	if (-1 == bind(sock,(struct sockaddr *)&locAddr, sizeof(struct sockaddr)))
    {
		perror("error bind failed");
		close(sock);
		exit(EXIT_FAILURE);
    } 
}

