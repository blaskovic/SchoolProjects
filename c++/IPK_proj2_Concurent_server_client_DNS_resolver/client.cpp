#include <stdio.h>
#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>

using namespace std;

enum ERRORS
{
	E_OK,
	E_GETHOST,
	E_INVALID_URL,
	E_SOCKET,
	E_CONNECT_WRITE,
};

class Client {

	char *web_url;
	int web_port;
	int error;
	char *ip_address;
	char *hostent_addr;
	int hostent_addr_len;
	string header;
	
	public:
    int set_url(string);
	int send_request(string);
};

int Client::set_url(string url)
{
	int pos = 0;
	web_port = 0;

	// Port
	if(url.find(":") != -1)
	{
		web_port = atoi(url.substr(url.find(":")+1).c_str());
		url = url.substr(0, url.find(":"));
	}
	else
	{
		return E_INVALID_URL;
	}
	
	// Copy url
	web_url = new char[url.size() + 1];
	web_url[url.size()] = 0;
	memcpy(web_url, url.c_str(), url.size());

	return E_OK;
}

int Client::send_request(string request)
{
	// Is valid URL?
	if(strcmp(web_url, "") == 0)
	{
		return E_INVALID_URL;
	}

	int s, n;
	struct sockaddr_in sin; struct hostent *hptr;
	char msg;
	int msg_end = 0;
	request += "\n\n";
	char *request_text = new char[request.size() + 1];
	request_text[request.size()] = 0;
	memcpy(request_text, request.c_str(), request.size());

	if((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		return E_SOCKET;
	}

	sin.sin_family = PF_INET;
	sin.sin_port = htons(web_port);

	if ((hptr = gethostbyname(web_url)) == NULL)
	{
		fprintf(stderr, "gethostname error: %s", web_url);
		return E_GETHOST;
	}

	memcpy( &sin.sin_addr, hptr->h_addr, hptr->h_length);

	if (connect (s, (struct sockaddr *)&sin, sizeof(sin) ) < 0 )
	{
		return E_CONNECT_WRITE;
	}

	if (write(s, request_text, strlen(request_text)) < 0 )
	{
		return E_SOCKET;
	}

	// Read message - header
	while((n = read(s, (char*)&msg, sizeof(msg))) > 0)
	{
		if(msg == '\n')
			msg_end++;
		else
			msg_end = 0;

		header += msg;

		if(msg_end == 2) break;
	}

	// Write the answer
	string line;
	while(header.find("\n") != 0)
	{
		line = header.substr(0, header.find("\n"));

		if(line.find("Err") == 0)
			cerr << line << endl;
		else
			cout << line << endl;

		header = header.substr(header.find("\n") + 1);
	}

	// Close connection
	if (close(s) < 0)
	{ 
		return E_SOCKET;
	}
	
	return E_OK;
}

/*
 *
 * Help
 *
 * */
void print_help()
{
	cout << "DNS Client by Branislav Blaskovic xblask00@fit.vutbr.cz" << endl;
	cout << "\t Usage: client server_host:port [-4][-6] domain_name" << endl;
}

/*
 *
 * Main function
 *
 * */
int main (int argc, char **argv)
{
	int error = E_OK, arg_index = 0;
	string request_message = "";
	int was_ipv4 = 0, was_ipv6 = 0;

	// Need help?
	if(argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
	{
		print_help();
		return EXIT_SUCCESS;
	}

	// Not correct args
	if(argc != 4 && argc != 5)
	{
		cerr << "Bad arguments" << endl;
		return EXIT_FAILURE;
	}

	Client request;

	// Set server url
	error = request.set_url(argv[1]);
	if(error != E_OK)
	{
		return error;
	}

	// What user want to do?
	for(int i = 2; i < (argc-1); i++)
	{
		// IPv4
		if(strcmp(argv[i], "-4") == 0 && !was_ipv4)
		{
			was_ipv4 = 1;
			request_message += "GET IPV4 ";
			request_message += argv[argc-1];
			request_message += "\n";
		}
		// IPv6
		else
		if(strcmp(argv[i], "-6") == 0 && !was_ipv6)
		{
			was_ipv6 = 1;
			request_message += "GET IPV6 ";
			request_message += argv[argc-1];
			request_message += "\n";
		}
		// Problem with arguments
		else
		{
			cerr << "Bad arguments" << endl;
			return EXIT_FAILURE;
		}
	}
	
	// Add end of request
	request_message += "CLOSE";

	// Send request
	error = request.send_request(request_message);

	return error;
}
