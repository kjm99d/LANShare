#pragma once
#include <map>
#include <vector>
#include <string>
#include <sstream>
using namespace std;
#pragma hdrstop

#include "TCPServer.h"

typedef struct _RequestHeader {
	string method;
	string url;
	string querystring;
	map<string, string> header;

	string payloads;
} RequestHeader;
typedef int (*fp_HTTPEvent)(SOCKET sock, string method, string uri);

class CHTTPServer : public CTCPServer
{
public:
	void Receive(fp_HTTPEvent fp);

private:
	bool Parse(const string& data, RequestHeader& ref);
	bool Split(vector<string>& ref, string src, std::string delimeter);
};

