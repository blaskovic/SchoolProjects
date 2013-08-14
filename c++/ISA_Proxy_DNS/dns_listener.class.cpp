/*******************************
 *
 * DNS Proxy
 * Author: Branislav Blaskovic
 * xblask00@stud.fit.vutbr.cz
 *
 * *****************************/

#include "dns_listener.class.h"

DNS_Listener::DNS_Listener()
{
    // Call statistic class
    stats = Statistic();
    
    // Set some default values
    server_port = 53;
    local_ip = "";
    timeout = 1000;
    remove_broken = false;

}

DNS_Listener::~DNS_Listener()
{
    // This destructor is called to cleanup the mess
    free(recv_data);
    free(recv_data_dns);
}

void DNS_Listener::set_port(in_port_t port)
{
    server_port = port;
}

int DNS_Listener::listen()
{
    // Prepare socket
    int socket_client = udt_init(server_port);

    // Prepare buffers
    recv_data = (uint16_t*)malloc(PACKET_MAX);
    if(recv_data == NULL)
    {
        cerr << "Error while malloc() buffer\n" << endl;
        return 1;
    }
    // Receive from DNS
    recv_data_dns = (uint16_t*)malloc(PACKET_MAX);
    if(recv_data_dns == NULL)
    {
        free(recv_data);
        cerr << "Error while malloc() dns buffer\n" << endl;
        return 1;
    }

    // Set the first DNS to server to begining of list
    actual_server = server_list.begin();

    while(1)
    {
        struct sockaddr_in sa_client, sa_dns;
        int n = udt_recv(socket_client, recv_data, PACKET_MAX, NULL, NULL, &sa_client);

        if(n > 0)
        {
            // Do dirty things to packet
            Packet packet_obj = Packet();
            packet_obj.set_packet((uint16_t*)recv_data);

            // Add to statistics
            stats.add(packet_obj.get_domain(), inet_ntoa(sa_client.sin_addr), packet_obj.get_type(), *actual_server);

            // Send to DNS 
            int socket_dns = socket(AF_INET, SOCK_DGRAM, 0);
            fcntl(socket_dns, F_SETFL, O_NONBLOCK);
            if (socket_dns <= 0) {
                fprintf(stderr, "Cannot create DNS socket.\n");
                continue;
            }

            if(!udt_send(socket_dns, 53, recv_data, n))
            {
                fprintf(stderr, "Sending to DNS failed\n");
                continue;
            }


            // Wait for response
            // and calculate the timeout
            struct timeval tim;
            struct timeval cycle;
            gettimeofday(&tim, NULL);
            double t1 = tim.tv_sec + (tim.tv_usec/1000000.0);
            double t2;
            int n_dns = 0;
            while(n_dns == 0)
            {
                // Receive respond
                n_dns = udt_recv(socket_dns, recv_data_dns, PACKET_MAX, NULL, NULL, &sa_dns);

                // Check for timeout (we have it in miliseconds, so convert it to micro)
                gettimeofday(&tim, NULL);
                t2 = tim.tv_sec + (tim.tv_usec/1000000.0);
                if((t2 - t1) > (double)(timeout / 1000.0))
                {
                    cerr << "DNS server '" << *actual_server << "' timed out." << endl;
                    
                    // Does user want to remove broken servers?
                    if(remove_broken)
                    {
                        if(server_list.size() > 1)
                        {
                            server_list.erase(actual_server);
                            cerr << "Server '" << *actual_server << "' was removed from list of servers" << endl;
                        }
                        else
                        {
                            cerr << "You want to remove broken servers and '" << *actual_server << "' is broken, but it's the last in the list, so I will leave it here." << endl;
                        }
                    }   

                    break;
                }
            }

            // Set the actual server on another position or loop it if it's the last one
            actual_server++;
            if(actual_server == server_list.end())
            {
                actual_server = server_list.begin();
            }

            // Send back to client
            if(n_dns > 0)
            {
                int sent_size = sendto(socket_client, recv_data_dns, n_dns, 0, (sockaddr *)&sa_client, sizeof(sa_client));
            }
        }
        usleep(10000);
    }


    return 0;
}

inline int DNS_Listener::udt_init(in_port_t local_port)
{
    int udt = socket(AF_INET, SOCK_DGRAM, 0);
    fcntl(udt, F_SETFL, O_NONBLOCK);
    if (udt <= 0) {
        cerr << "Cannot create UDT descriptor." << endl;
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = (local_ip.compare("") == 0) ? INADDR_ANY : inet_addr(local_ip.c_str());
    sa.sin_port = htons(local_port);
    int err = bind(udt, (const struct sockaddr *) &sa, sizeof(sa));
    if (err == -1) {
        cerr << "Cannot bind to the specified port." << endl;
        exit(EXIT_FAILURE);
    }
    return udt;
}

inline int DNS_Listener::udt_send(int udt, /*in_addr_t addr,*/ in_port_t port, void *buff, size_t nbytes)
{
    struct stat info;
    if (fstat(udt, &info) != 0) {
        cerr << "descriptor is invalid, probably udt_init not called.\n" << endl;
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;

    // User server from list on actual position
    sa.sin_addr.s_addr = inet_addr((*actual_server).c_str());
    sa.sin_port = htons(port);
    ssize_t nsend = sendto(udt, buff, nbytes, 0, (const struct sockaddr *) &sa, sizeof(sa));

    return nsend == nbytes;
}

inline int DNS_Listener::udt_recv(int udt, void *buff, size_t nbytes, in_addr_t *addr, in_port_t *port, struct sockaddr_in *sa)
{
    struct stat info;
    if (fstat(udt, &info) != 0) {
        cerr << "Descriptor is invalid, probably udt_init not called.\n" << endl;
        exit(EXIT_FAILURE);
    }
    //struct sockaddr_in sa;
    memset(sa, 0, sizeof(*sa));
    socklen_t salen = sizeof(*sa);
    ssize_t nrecv = recvfrom(udt, buff, nbytes, 0, (struct sockaddr *) sa, &salen);
    if(addr != NULL) (*addr) = ntohl(sa->sin_addr.s_addr);
    if(port!=NULL) (*port) = ntohs(sa->sin_port);
    if (nrecv < 0 ) nrecv = 0;
    return nrecv;
}
