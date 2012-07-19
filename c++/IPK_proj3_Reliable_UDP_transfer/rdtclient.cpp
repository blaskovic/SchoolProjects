//=============================================================================
// Author      : Branislav Blaskovic
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <string.h>
#include <fcntl.h>
#include <vector>
#include <time.h>
#include <signal.h>
#include "udt.h"
#include "Base64Encoder.h"
#include "xmlparser.h"

#define MAXLINE 500
#define PROGRAM "rdtclient"
#define PROGRAM_INFO "RDT Client by xblask00\n\n"
#define READ_LIMIT 300
#define WINDOW_SIZE 10
#define TIME_LIMIT 2

using namespace std;

in_addr_t remote_addr = 0x7f000001;
in_port_t remote_port = 0;
in_port_t local_port = 0;

void shutdown(int signum)
{
	cerr << "[ CAUGHT SIGNAL: " << signum << " ]" << endl;
	cerr << "[ SHUTTING DOWN ]" << endl;
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv )
{
	// Catch signals
	signal(SIGTERM, shutdown);
	signal(SIGINT, shutdown);

	char ch;
	char buf[80];
	char sendline[MAXLINE];
	char recvline[MAXLINE];
	int udt;

	while ((ch = getopt(argc,argv,"s:d:h")) != -1)
	{
		switch(ch)
		{
		case 's':
				local_port = atol(optarg);
			break;
		case 'd':
				remote_port = atol(optarg);
			break;
		case 'h':
			fprintf(stdout, PROGRAM_INFO);
			fprintf(stdout, "usage: rdtclient -s port -d port\n\n");
			fprintf(stdout, "  s port    : local UDP socket binds to `port'\n" );
			fprintf(stdout, "  d port    : UDP datagrams will be sent to the remote `port'\n" );
			exit(EXIT_SUCCESS);
		}
	}

	if (remote_addr == 0 || remote_port == 0 || local_port == 0)
	{
		fprintf(stderr, "Missing required arguments! Type '%s -h' for help.\n", PROGRAM);
		exit(EXIT_FAILURE);
	}

	udt = udt_init(local_port);

	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);	// make stdin reading non-clocking

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(udt, &readfds);
	FD_SET(STDIN_FILENO, &readfds);

	string to_send = "";

	int loaded = 0;
	int c;
	int packet_id = 1;
	int reading = 1;
	int missing_ack = 1;
	int last_packet_ack = 0;

	vector<string> packets_xml;
	vector<int> packets_ack;
	vector<int> packets_time;
	vector<string> sliding_window;

	while (select(udt+1, &readfds, NULL, NULL, NULL))
	{
		// READING stdin stream
		if(reading && FD_ISSET(STDIN_FILENO, &readfds))
		{
			c = fgetc(stdin);
			to_send += c;

			if(c == EOF)
				reading = 0;

			loaded++;
				
			if(loaded == READ_LIMIT || reading == 0)
			{
				if(reading == 0)
					packet_id = 0;

				string str_encoded = base64_encode(reinterpret_cast<const unsigned char*>(to_send.c_str()), to_send.length());
				string xml = create_segment(packet_id, str_encoded);
				packets_xml.push_back(xml);
				packets_ack.push_back(0);
				packets_time.push_back(-1);
				
				to_send = "";
				loaded = 0;
				packet_id++;
			}
		}
	
		// RECEIVING ACKSs
		if (FD_ISSET(udt, &readfds))
		{
			int n = udt_recv(udt, recvline, MAXLINE, NULL, NULL);
			recvline[n] = 0;

			unsigned packet_id = get_packet_id(recvline);

			// Find packet and set it's ack
			for(int i = 0; i < packets_xml.size(); i++)
			{
				if(get_packet_id(packets_xml[i]) == packet_id)
				{
					packets_ack[i] = 1;
					// Remove this
					if(packet_id != 0)
					{
						break;
					}
				}
			}
		}

		// SLIDING WINDOW filling
		missing_ack = 0;
		for(size_t i = 0; i < packets_xml.size(); i++)
		{
			// Full sliding window
			if(sliding_window.size() == WINDOW_SIZE)
				break;

			int fill_it = 1;
			// Don't want last packet yet
			if(get_packet_id(packets_xml[i]) == 0)
				continue;
			
			// Have ack?
			if(packets_ack[i] == 1)
				continue;

			// Is here missing ACK packet?
			if(packets_ack[i] == 0)
				missing_ack = 1;
			
			// Is in window already? Have ACK?
			// If I comment this for() cycle out, it's fast as hell! but with some random seg faults :)
			for(size_t a = 0; a < sliding_window.size(); a++)
			{
				if(get_packet_id(sliding_window[a]) == get_packet_id(packets_xml[i]))
					fill_it = 0;
			}
			// Doesn't have ACK, not in window and timer is -1 or more then limit
			if(
					packets_ack[i] == 0 && 
					fill_it == 1 && 
					(packets_time[i] == -1 || (packets_time[i] > 0 && (packets_time[i] + TIME_LIMIT) < time(NULL)))
			)
			{
				cerr << "[ FILLING WINDOW ]" << endl;
				sliding_window.push_back(packets_xml[i]);
			}
		}
	
		// SLIDING WINDOW sending
		if(sliding_window.size() == WINDOW_SIZE || (sliding_window.size() > 0 && reading == 0))
		{
			cerr << "[ SENDING WINDOW ]" << endl;
			for(size_t i = 0; i < sliding_window.size(); i++)
			{
				//cerr << sliding_window[i] << endl;
				packets_time[get_packet_id(sliding_window[i]) - 1] = time(NULL);
				if(!udt_send(udt, remote_addr, remote_port, (void *)sliding_window[i].c_str(), sliding_window[i].size()))
				{
					cerr << "Error: packet sending failed" << endl;
				}
			}
			sliding_window.clear();
			// Clear ACKS
			for(int i = 0; i < packets_xml.size(); i++)
			{
				if(packets_ack[i] == 1)
				{
					packets_xml.erase(packets_xml.begin() + i);
					packets_ack.erase(packets_ack.begin() + i);
					packets_time.erase(packets_time.begin() + i);
				}
			}
		}
		
		// Have all ACKS! Let's send last packet
		if(missing_ack == 0 && reading == 0)
		{
			// Do we have last packet?
			for(int i = 0; i < packets_xml.size(); i++)
			{
				if(get_packet_id(packets_xml[i]) == 0)
				{	
					// Have ack?
					if(packets_ack[i] == 1)
					{
						last_packet_ack = 1;
						break;
					}
					// Need to be sent?
					if(packets_time[i] == -1 || (packets_time[i] > 0 && (packets_time[i] + TIME_LIMIT) < time(NULL)))
					{
						cerr << "[ SENDING LAST PACKET ]" << endl;
						packets_time[i] = time(NULL);
						if(!udt_send(udt, remote_addr, remote_port, (void *)packets_xml[i].c_str(), packets_xml[i].size()))
						{
							cerr << "Error: packet sending failed" << endl;
						}
					}
				}
			}
		}

		// We have all ACKS!!!!
		if(last_packet_ack)
		{
			cerr << "[ ALL ACKS RECEIVED ]" << endl;
			cerr << "[ EXITING NOW! ]" << endl;
			exit(EXIT_SUCCESS);
		}

		FD_ZERO(&readfds);
		FD_SET(udt, &readfds);
		FD_SET(STDIN_FILENO, &readfds);
	}
	return EXIT_SUCCESS;
}
