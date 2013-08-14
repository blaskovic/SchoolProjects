/*******************************
 *
 * DNS Proxy
 * Author: Branislav Blaskovic
 * xblask00@stud.fit.vutbr.cz
 *
 * *****************************/

#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#include "dns_listener.class.h"

#define DNS_DEFAULT_PORT 53

DNS_Listener *server;

void signal_catch(int signal)
{
    // Handle signals which we need to handle :)
    switch(signal)
    {
        case SIGSEGV:
            
            delete server;
            cerr << "An error has occured. I have to quit this program." << endl;
            exit(1);

        break;

        case SIGUSR1:

            server->stats.print_log();

        break;

        case SIGINT:
        case SIGTERM:

            delete server;
            exit(0);

        break;
    }
}

void print_help()
{
    cout << "DNS Proxy by Branislav Blaskovic (xblask00@stud.fit.vutbr.cz)" << endl;
    cout << "\tSynopsis: dns_stat -l <ip> -p <port> -s <server>" << endl;
    cout << "\t-type - statistics by type" << endl;
    cout << "\t-source - statistics by source" << endl;
    cout << "\t-destination - statistics by destination" << endl;
    cout << "\t-hour - add statistics for last hour" << endl;
    cout << "\t-average - add average statistics for hour" << endl;
    cout << "\t-histogram - histogram for hours of day" << endl;
    cout << "\t-timeout - time in milliseconds for DNS server timeout (default: 1000)" << endl;
    cout << "\t-remove-broken - if DNS server is broken (timeout), remove it from list" << endl;
    cout << "\tMultiple -s <server> arguments are allowed." << endl;
}

int main (int argc, char **argv)
{
    // No param? Help!
    if(argc == 1)
    {
        print_help();
        return EXIT_SUCCESS;
    }

    server = new DNS_Listener();

    // Catch signal
    signal(SIGUSR1, &signal_catch);
    signal(SIGINT, &signal_catch);
    signal(SIGSEGV, &signal_catch);

    // Get arguments
    int i;
    char *end_ptr;

    int port = DNS_DEFAULT_PORT;
    int stat_by_type = 0, stat_by_source = 0, stat_by_destination = 0;
    string ip_addr = "";

    // Load arguments
    for(i = 1; i < argc; i++)
    {
        // -p <port>
        if(!strcmp("-p", argv[i]) && (i+1) != argc)
        {
            i++;
            port = strtol(argv[i], &end_ptr, 10);
            if(strlen(end_ptr) != 0)
            {
                cerr << "Invalid port" << endl;
                return EXIT_FAILURE;
            }
        }

        // -l <ip> 
        if(!strcmp("-l", argv[i]) && (i+1) != argc)
        {
            server->local_ip = string(argv[i+1]);
            i++;
        }

        // -s <ip> 
        if(!strcmp("-s", argv[i]) && (i+1) != argc)
        {
            server->server_list.push_back(string(argv[i+1]));
            i++;
        }

        // -type
        if(!strcmp("-type", argv[i]))
            server->stats.stat_type.type = 1;

        // -source
        if(!strcmp("-source", argv[i]))
            server->stats.stat_type.source = 1;

        // -destination
        if(!strcmp("-destination", argv[i]))
            server->stats.stat_type.destination = 1;
        
        // -average
        if(!strcmp("-average", argv[i]))
            server->stats.stat_type.average = 1;

        // -hour
        if(!strcmp("-hour", argv[i]))
            server->stats.stat_type.hour = 1;

        // -histogram
        if(!strcmp("-histogram", argv[i]))
            server->stats.stat_type.histogram = 1;

        // -timeout
        if(!strcmp("-timeout", argv[i]) && (i+1) != argc)
        {
            // Check for correct timeout interval
            if(atoi(argv[i+1]) > 0)
            {
                server->timeout = atoi(argv[i+1]);
            }
            else
            {
                cerr << "Invalid timeout. Using default timeout of " << server->timeout << " milliseconds" << endl;
            }
            i++;
        }

        // -remove-broken
        if(!strcmp("-remove-broken", argv[i]))
        {
           server->remove_broken = true;
        }

    }

    // Do we have at least one DNS server specified?
    if(server->server_list.size() == 0)
    {
        cerr << "Argument -s <server> is required." << endl;
        return EXIT_FAILURE;
    }

    // Listen to connections
    server->set_port(port);
    server->listen();

    return EXIT_SUCCESS;
}
