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
#include <signal.h>
#include <stdexcept>
#include "udt.h"
#include "Base64Encoder.h"
#include "xmlparser.h"

#define MAXLINE 500
#define PROGRAM "rdtserver"
#define PROGRAM_INFO "RDT SERVER by xblask00\n\n"

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
	string last_packet = "";

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
			fprintf(stdout, "usage: rdtserver -s port -d port\n\n");
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

	// It is important to init UDT!
	udt = udt_init(local_port);

	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);	// make stdin reading non-clocking

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(udt, &readfds);
	FD_SET(STDIN_FILENO, &readfds);

	string to_send = "";

	char tmp[1];
	int loaded = 0;
	char c;
	int packet_id = 1;
	int reading = 1;
	int temp_max = 0;
	vector<string> packets;
	vector<string> print;

	while (select(udt+1, &readfds, NULL, NULL, NULL))
	{
		if (FD_ISSET(udt, &readfds))
		{
			int n = udt_recv(udt, recvline, MAXLINE, NULL, NULL);
			recvline[n] = 0;

			int packet_id = get_packet_id(recvline);
			string data = get_packet_data(recvline);

			if(packet_id > temp_max)
			{
				temp_max = packet_id;
			}

			cerr << "[ RECEIVED PACKET ]" << endl;

			packets.push_back(recvline);
			
			// Send ACK for this packet
			string ack_xml = create_segment(packet_id, "ACK");
			if(!udt_send(udt, remote_addr, remote_port, (void *)ack_xml.c_str(), ack_xml.size()))
			{
				perror("Error: packet sending failed");
			}

			// Was the last one?
			if(packet_id == 0)
			{
				cerr << "[ ALL PACKETS RECEIVED - PRINT IT! ]" << endl;
				
				// Print the file
				for(int i = 1; i <= temp_max; i++)
				{
					// Find packet
					for(unsigned a = 0; a < packets.size(); a++)
					{
						// Got packet!
						if(get_packet_id(packets[a]) == i)
						{
							string decoded = base64_decode(get_packet_data(packets[a]));
							fwrite(decoded.c_str(), decoded.length(), 1, stdout);
							packets.erase(packets.begin() + a);
							break;
						}
					}
				}

				// Find last packet
				string decoded = base64_decode(get_packet_data(packets[packets.size()-1]));
				fwrite(decoded.c_str(), decoded.length()-1, 1, stdout);
				
				// Clean stuff
				packets.clear();
				print.clear();
				temp_max = 0;
				last_packet = "";
				fflush(stdout);
				cerr << "[ DATA WRITTEN TO STDOUT ]" << endl;
			}
		}

		FD_ZERO(&readfds);
		FD_SET(udt, &readfds);
		FD_SET(STDIN_FILENO, &readfds);
	}
	return EXIT_SUCCESS;
}
