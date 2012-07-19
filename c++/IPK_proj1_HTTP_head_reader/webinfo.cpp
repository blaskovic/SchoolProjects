/*
 *
 * Webinfo for IPK proj1
 * by Branislav Blaskovic
 * xblask00@stud.fit.vutbr.cz
 *
 * */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAX_PARAMS 4

using namespace std;

enum ERRORS
{
	E_OK,
	E_GETHOST,
	E_INVALID_URL,
	E_REDIRECT_LOOP,
	E_SOCKET,
	E_CONNECT_WRITE,
	E_PROTOCOL,
};

/*
 *
 * Class Webinfo
 * - get info from web address
 *
 * */

class Webinfo {

	/*
	 *
	 * Variables in class
	 *
	 * */
	
	char *web_url;
	char *web_path;
	int web_port;
	char *ip_address;
	char *hostent_addr;
	int hostent_addr_len;
	string header;
	int redirect_num;
	
	/*
	 *
	 * Functions in class
	 *
	 * */
	public:
    void set_url(string);
	void print_url();
	void print_ip();
	int head_request();
	int print_answer(char*, int);
	string get_header_row(string);
	Webinfo();
};


Webinfo::Webinfo()
{
	redirect_num = 0;
}

string Webinfo::get_header_row(string search)
{
	string tmp = header;
	int pos = 0;
	
	search += ": ";

	pos = tmp.find(search);
	
	// Not found
	if(pos == -1) return search + "N/A\n";

	// Parsing
	tmp = tmp.substr(pos);
	// Line endings
	pos = tmp.find("\n");

	return tmp.substr(0, pos) + "\n";
}

int Webinfo::print_answer(char *args, int max_params)
{
	// Error message
	if(header.find("HTTP/1.1 200") == -1)
	{
		cerr << "Not code 200" << endl;
		cerr << header.substr(0, header.find("\n")) << endl;
		return E_PROTOCOL;
	}
	
	// No param, print whole head
	if(args[0] == 0)
	{
		cout << header;
	}

	// Print in correct order
	for(int i = 0; i < max_params; i++)
	{
		switch(args[i])
		{
			case 'l':
				cout << get_header_row("Content-Length");
			break;
			case 's':
				cout << get_header_row("Server");
			break;
			case 'm':
				cout << get_header_row("Last-Modified");
			break;
			case 't':
				cout << get_header_row("Content-Type");
			break;
		}
	}

	return E_OK;
}

void Webinfo::set_url(string url)
{
	int pos = 0;
	string path = "/";
	web_port = 80;
	string port;

	// Finding http://
	pos = url.find("http://");
	
	if(pos != 0)
	{
		url = "";
	}
	else
	{
		url = url.substr(string("http://").size());

		// getting path
		pos = url.find("/");
		if(pos > 0)
		{
			path = url.substr(pos);
			url = url.substr(0, pos);
		}

		// port
		if(url.find(":") != -1)
		{
			web_port = atoi(url.substr(url.find(":")+1).c_str());
			url = url.substr(0, url.find(":"));
		}
	}

	web_url = new char[url.size() + 1];
	web_url[url.size()] = 0;
	memcpy(web_url, url.c_str(), url.size());

	web_path = new char[path.size() + 1];
	web_path[path.size()] = 0;
	memcpy(web_path, path.c_str(), path.size());
}

void Webinfo::print_url()
{
	cout << web_url << endl;
}

void Webinfo::print_ip()
{
	cout << ip_address << endl;
}

int Webinfo::head_request()
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
	string text = "";
	text += "HEAD ";
	text += web_path;
	text += " HTTP/1.1\r\nHost: ";
	text += web_url;
	text += "\r\nConnection: close\r\n\r\n";
	char *request_text = new char[text.size() + 1];
	request_text[text.size()] = 0;
	memcpy(request_text, text.c_str(), text.size());
	
	// Connection
	if ( (s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) < 0)
	{
		return E_SOCKET;
	}

	sin.sin_family = AF_INET;
	sin.sin_port = htons(web_port);

	if ((hptr =  gethostbyname(web_url)) == NULL)
	{
		close(s);
		return E_GETHOST;
	}

	memcpy( &sin.sin_addr, hptr->h_addr, hptr->h_length);

	if (connect (s, (struct sockaddr *)&sin, sizeof(sin) ) < 0 )
	{
		close(s);
		return E_CONNECT_WRITE;
	}

	if ( write(s, request_text, strlen(request_text)) < 0 )
	{
		close(s);
		return E_CONNECT_WRITE;
	}

	// Read message - header
	while((n = read(s, (char*)&msg, sizeof(msg))) > 0) {
		if(msg == '\r') continue;
		if(msg == '\n')
			msg_end++;
		else
			msg_end = 0;

		if(msg_end == 2) break;

		header += msg;
	}

	// Close connection
	if(close(s) < 0) { 
		return E_SOCKET;
	}

	// Redirecting?
	int pos = 0;
	string tmp = "", new_url = "";
	
	pos = header.find("HTTP/1.1 302");
	if(pos == -1) pos = header.find("HTTP/1.1 301");

	if(pos == 0)
	{
		pos = header.find("Location: ");
		if(pos == -1)
		{
			return E_INVALID_URL;
		}

		tmp = header.substr(pos);
		// end of line
		tmp = tmp.substr(string("Location: ").size());
		new_url = tmp.substr(0, tmp.find("\n"));

		// Redirect it!
		if(redirect_num == 4)
		{
			return E_REDIRECT_LOOP;
		}	
		header = "";
		redirect_num++;
		set_url(new_url);
		return head_request();
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
	cout << "Webinfo by Branislav Blaskovic xblask00@stud.fit.vutbr.cz" << endl;
	cout << "\t -l - size of content" << endl;
	cout << "\t -s - server identification" << endl;
	cout << "\t -m - date of last modification" << endl;
	cout << "\t -t - type of content" << endl;
}

/*
 *
 * Main function
 *
 * */
int main (int argc, char **argv)
{

	int ch, l_flag = 0, s_flag = 0, m_flag = 0, t_flag = 0, h_flag = 0;
	int error = E_OK, arg_index = 0;
	char args[MAX_PARAMS] = {0};
	
	while ((ch = getopt(argc, argv, "hlsmt")) != -1) 
	{	
		switch (ch)
		{
			case 'l':
				if(arg_index < MAX_PARAMS && !l_flag)
					args[arg_index++] = 'l';
				l_flag = 1;
			break;
			case 's':
				if(arg_index < MAX_PARAMS && !s_flag)
					args[arg_index++] = 's';
				s_flag = 1;
			break;
			case 'm':
				if(arg_index < MAX_PARAMS && !m_flag)
					args[arg_index++] = 'm';
				m_flag = 1;
			break;
			case 't':
				if(arg_index < MAX_PARAMS && !t_flag)
					args[arg_index++] = 't';
				t_flag = 1;
			break;
			case 'h':
				h_flag = 1;
			break;
			default:
				cerr << "Error: Bad arguments" << endl;
				return 1;
			break;
		}
	}
	argc -= optind;
	argv += optind;
	
	// Help needed!
	if(h_flag == 1)
	{
		// Was another argument given?
		if(arg_index != 0 || argc != 0)
		{
			cerr << "Error: Bad arguments" << endl;
			return EXIT_FAILURE;
		}

		print_help();
		return EXIT_SUCCESS;
	}

	// Don't have address
	if(argc != 1)
	{
		cerr << "Error: Bad arguments" << endl;
		return EXIT_FAILURE;
	}

	Webinfo request;
	request.set_url(argv[0]);
	error = request.head_request();

	if(error == E_OK)
	{
		return request.print_answer(args, MAX_PARAMS);
	}
	else
	{
		switch(error)
		{
			case E_GETHOST:
				cerr << "Error: Gethostbyname failed." << endl;
			break;

			case E_REDIRECT_LOOP:
				cerr << "Error: Redirection looop." << endl;
			break;

			case E_INVALID_URL:
				cerr << "Error: Invalid URL" << endl;
			break;

			case E_SOCKET:
				cerr << "Error: Creating socket" << endl;
			break;

			case E_CONNECT_WRITE:
				cerr << "Error: While connect() or write()" << endl;
			break;
		}
		return error;
	}

	return EXIT_SUCCESS;
}
