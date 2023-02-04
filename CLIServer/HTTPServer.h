#pragma once
#include <map>
#include <vector>
#include <string>
#include <sstream>
using namespace std;


#include <json/json.h>
#pragma hdrstop

#include "HTTPJsonResponse.h"
#include "HTTPResponse.h"
#include "TCPServer.h"

class ResponseDispatcher;

typedef struct _RequestHeader {
	string method;
	string url;
	map<string, string> querystring;
	map<string, string> header;

	map<string, string> queryPayloads;
	Json::Value jsonPayloads;
} RequestHeader;


// V2 지원 콜백 함수 원형
typedef IHTTPResponse * (*fp_HTTPGetControllerV2)(CTCPServer& tcp, string uri, std::map<string, string> querystring, ResponseDispatcher& dispatcher);

// V1 지원 콜백 함수 원형
typedef string (*fp_HTTPGetController)(CTCPServer& tcp, string uri, std::map<string, string> querystring);
typedef string (*fp_HTTPPostController)(CTCPServer& tcp, string uri, std::map<string, string> querystring, map<string, string> queryPayloads, Json::Value jsonPayloads);

class ResponseDispatcher
{
	/**
	 * 여기서 dispatcher 에서 JSON(...) 함수로 JSON응답 객체 받아서 처리 해주면 ..
	 */
public:
	ResponseDispatcher() : response(nullptr) {} ;
	~ResponseDispatcher() { delete response; response = nullptr; };

public:
	IHTTPResponse* JSON(Json::Value json, string cors = "")
	{
		response = new CHTTPJsonResponse(json);
		response->SetCORS(cors);
		return response;
	}

private:
	IHTTPResponse * response;
	
};

class CHTTPServer : public IServer
{
public:
	void SetController(fp_HTTPGetController		fp);
	void SetController(fp_HTTPPostController	fp);
	void SetController(fp_HTTPGetControllerV2	fp);

public:
	bool Receive(CTCPServer& tcp);
	bool ReceiveV2(CTCPServer& tcp);

private:
	string TcpCallbackSelectorFromV1(CTCPServer& tcp);
	IHTTPResponse* TcpCallbackSelectorFromV2(CTCPServer& tcp);

private:
	bool Parse(const string& data, RequestHeader& ref);
	bool Split(vector<string>& ref, string src, std::string delimeter);
	

	fp_HTTPGetController	fp_GetController;
	fp_HTTPGetControllerV2	fp_GetControllerV2;
	fp_HTTPPostController	fp_PostController;

private:
	SOCKET m_client; // 현재 소켓
	RequestHeader header; // 헤더

private:
	ResponseDispatcher dispatcher;
};

