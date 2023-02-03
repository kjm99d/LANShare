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
	map<string, string> querystring;
	map<string, string> header;

	string payloads;
} RequestHeader;

// typedef int (*fp_HTTPEvent)(SOCKET sock, string method, string uri, string& responseBody);
typedef int (*fp_HTTPGetController)(CTCPServer& tcp, SOCKET sock, string method, string uri, std::map<string, string> querystring, string& responseBody);

class CHTTPServer : public IServer
{
public:
	bool Receive(CTCPServer& tcp, fp_HTTPGetController fp_callback);

private:
	bool Parse(const string& data, RequestHeader& ref);
	bool Split(vector<string>& ref, string src, std::string delimeter);

	fp_HTTPGetController fp_GetController;
};

