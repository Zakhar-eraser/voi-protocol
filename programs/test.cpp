#include "get_data.h"
#include "anti-uav_link.h"

GetSock s("127.0.0.1", 14552, "127.0.0.1", 14553);


Packet testPacket;

int main()
{   

    Packet msg = {0};
    msg.gps.command = 1;
    msg.gps.lat = 11;
    msg.gps.lon = 5;
    msg.gps.alt = 12;

    Packet msg1 = {0};
    msg1.gps.command = 2;
    msg1.gps.lat = 10;
    msg1.gps.lon = 5;
    msg1.gps.alt = 10;


    s.startListening(s, 10);

    s.startMessaging(msg, s, 100);

    sleep(5);

    s.updateMessaging(msg1);

    State state;

    for (;;)
    {
        state = s.getCurrentState();
        std::cout << state.x << std::endl;
        sleep(1);
        // continue;
    }

}