/*******************************
 *
 * DNS Proxy
 * Author: Branislav Blaskovic
 * xblask00@stud.fit.vutbr.cz
 *
 * *****************************/

#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>


#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <signal.h>

#include <fcntl.h>

#ifdef __FreeBSD__
#include <sys/endian.h>
#else
#include <endian.h>
#endif

#define PACKET_MAX 65535

using namespace std;

typedef struct packet_data
{
    uint16_t id;
    uint16_t stuff;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} TPacket_data;

class Packet
{
    TPacket_data *packet;
    string domain_name;

    uint16_t qtype;

    void read_packet();
    uint8_t get_piece(TPacket_data*, int);
    
    public:
        Packet();
        void set_packet(uint16_t *);
        string get_domain();
        string get_type();
};

