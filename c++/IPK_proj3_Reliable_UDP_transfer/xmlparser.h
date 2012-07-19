//=============================================================================
// Author      : Branislav Blaskovic
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

// Get packet id from XML
int get_packet_id(string xml)
{
	string id = xml.substr(string("<rdt-segment id=\"xblask00\"><header>").size());
	id = id.substr(0, id.find(string("</header>")));
	return atoi(id.c_str());
}

// Get data from XML
string get_packet_data(string xml)
{
	string data = xml.substr(xml.find(string("<data>")) + string("<data>").size());
	data = data.substr(0, data.find(string("</data>")));
	return data;
}

// Create XML segment packet
string create_segment(int packet_id, string data)
{
	char number[1000];
	sprintf(number, "%d", packet_id);
	string segment = "<rdt-segment id=\"xblask00\"><header>" + string(number) + "</header><data>" + data + "</data></rdt-segment>";
	return segment;
}

