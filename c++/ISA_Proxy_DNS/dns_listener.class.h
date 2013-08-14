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
#include <sys/time.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include "packet.class.h"
#include "statistic.class.h"

#define PACKET_MAX 65535

class DNS_Listener
{
    in_port_t server_port;
    list<string>::iterator actual_server;
    
    int udt_init(in_port_t local_port);
    int udt_send(int udt, in_port_t port, void *buff, size_t nbytes);
    int udt_recv(int udt, void *buff, size_t nbytes, in_addr_t *addr, in_port_t *port, struct sockaddr_in *sa);
    uint16_t *recv_data;
    uint16_t *recv_data_dns;

    public:
        int timeout;
        bool remove_broken;
        list<string> server_list;
        string local_ip;
        Statistic stats;
        DNS_Listener();
        int listen();
        void set_port(in_port_t);
        ~DNS_Listener();
};
