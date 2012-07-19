#include <stdio.h>
#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <signal.h>

using namespace std;

// Global socket variable
int s;

void shutdown(int signum)
{
	cerr << "Caught signal: " << signum << endl;
	cerr << "Shutting the server down..." << endl;
	close(s);
	exit(0);
}

string get_ip(int type, string host)
{
	struct addrinfo hints, *res;
	int error;
	char addrstr[100];
	void *ptr;

	memset (&hints, 0, sizeof (hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags |= AI_CANONNAME;

	string fail = (type == 4) ? "Err4: Nenalezeno." : "Err6: Nenalezeno.";

	error = getaddrinfo ((const char*)host.c_str(), NULL, &hints, &res);
	if (error != 0)
	{
		return fail;
	}

	while (res)
	{
		inet_ntop (res->ai_family, res->ai_addr->sa_data, addrstr, 100);

		switch (res->ai_family)
		{
			case AF_INET:
				ptr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
			break;
			case AF_INET6:
				ptr = &((struct sockaddr_in6 *) res->ai_addr)->sin6_addr;
			break;
		}
		inet_ntop (res->ai_family, ptr, addrstr, 100);

		// Returning addresses
		if(type == 6 && res->ai_family == PF_INET6)
		{
			return addrstr;
		}
		else
		if(type == 4 && res->ai_family == PF_INET)
		{
			return addrstr;
		}

		res = res->ai_next;
	}

	return fail;
}

int receive(char *port)
{
	int t, sinlen;
	struct sockaddr_in sin;
	int i;
	char msg[1024];
	struct hostent * hp;
	int j;
	
	// Creating socket
	if((s = socket(PF_INET, SOCK_STREAM, 0 )) < 0)
	{
		cerr << "Error: scoket()" << endl;
		return EXIT_FAILURE;
	}

	// Configure
	sin.sin_family = PF_INET;              
	sin.sin_port = htons(atoi(port));
	sin.sin_addr.s_addr  = INADDR_ANY;   
	
	// Binding
	if (bind(s, (struct sockaddr *)&sin, sizeof(sin) ) < 0 ) {
		cerr << "Error: bind()" << endl;
		close(s);
		return EXIT_FAILURE;
	}

	// Start listening
	if (listen(s, 5)) { 
		cerr << "Error: listen()" << endl;
		close(s);
		return EXIT_FAILURE;
	}

	sinlen = sizeof(sin);
	while (1) {

		if ((t = accept(s, (struct sockaddr *) &sin, (socklen_t*)&sinlen) ) < 0 ) {
			cerr << "Error: accept()" << endl;
			close(s);
			return EXIT_FAILURE;
		}

		// Fork - parent process continue listening
		if(fork() != 0)	continue;

		hp = (struct hostent *)gethostbyaddr((char *)&sin.sin_addr,4,AF_INET);
		j = (int)(hp->h_length);

		bzero(msg,sizeof(msg));

		if(read(t, msg, sizeof(msg)) <0)
		{  
			cerr << "Error: read()" << endl;
			close(s);
			return EXIT_FAILURE;
		}
	
		// Prepare variables
		string request = msg;
		string host;
		string ip_address = "";
		string to_write = "";
		int pos = -1;
		int type = 0;

		cerr << "|*****************|" << endl;
		cerr << "|     REQUEST     |" << endl;
		cerr << request;
		cerr << "|-----------------|" << endl;

		// Loop through requeset message and parse, what user want
		while(request.find("CLOSE") != 0)
		{
			// Want IPv4?
			if((pos = request.find("GET IPV4")) == 0)
			{
				cerr << "| Getting IPv4    |" << endl;
				request = request.substr(string("GET IPV4 ").size());
				type = 4;
			}
			else
			// Want IPv6
			if((pos = request.find("GET IPV6")) == 0)
			{
				cerr << "| Getting IPv6    |" << endl;
				request = request.substr(string("GET IPV6 ").size());
				type = 6;
			}

			// Preparing IP addresses for sending back to client
			if(type == 4 || type == 6)
			{
				pos = request.find("\n");
				host = request.substr(0, pos);

				ip_address = get_ip(type, host);
				to_write += ip_address;
				to_write += "\n";
			}
			
			// Get next line for the next loop
			request = request.substr(request.find("\n")+1);
		}

		cerr << "|-----------------|" << endl;
		cerr << "|    RESPOND      |" << endl;
		cerr << to_write;
		cerr << "|_________________|" << endl << endl;

		to_write += "\n";

		if(write(t, to_write.c_str(), to_write.size()) < 0)
		{
			cerr << "Error: write()" << endl;
			exit(EXIT_FAILURE);
		}

		if (close(t) < 0)
		{
			cerr << "Error: close()" << endl;
			exit(EXIT_FAILURE);
		}

		exit(EXIT_SUCCESS);

	}
	if (close(s) < 0)
	{
		cerr << "Error: close()" << endl;
		return EXIT_FAILURE;
	}
}

int main(int argc, char **argv)
{
	int error = 0;

	// Catch signals
	signal(SIGTERM, shutdown);
	signal(SIGINT, shutdown);
	signal(SIGCHLD, SIG_IGN);

	// Buffers for cleaner output
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	// Arguments
	if(argc != 3 || strcmp(argv[1], "-p") != 0)
	{
		cerr << "Bad arguments.\nUsage: ./server -p PORT" << endl;
		return EXIT_FAILURE;
	}

	// Start receiving
	error = receive(argv[2]);
	
	return error;
}
