#pragma once
#include <map>
#include <vector>
#include <string>
#include <sstream>
using namespace std;
#pragma hdrstop

#include "json/json.h"

#include "TCPServer.h"

typedef struct _RequestHeader {
	string method;
	string url;
	map<string, string> querystring;
	map<string, string> header;

	map<string, string> queryPayloads;
	Json::Value jsonPayloads;
} RequestHeader;

typedef int (*fp_HTTPGetController)(CTCPServer& tcp, SOCKET sock, string uri, std::map<string, string> querystring, string& responseBody);
typedef int (*fp_HTTPPostController)(CTCPServer& tcp, SOCKET sock, string uri, std::map<string, string> querystring, map<string, string> queryPayloads, Json::Value jsonPayloads, string& responseBody);

class CHTTPServer : public IServer
{
public:
	void SetController(fp_HTTPGetController fp);
	void SetController(fp_HTTPPostController fp);

	bool Receive(CTCPServer& tcp);

private:
	bool Parse(const string& data, RequestHeader& ref);
	bool Split(vector<string>& ref, string src, std::string delimeter);
	

	fp_HTTPGetController fp_GetController;
	fp_HTTPPostController fp_PostController;
};

