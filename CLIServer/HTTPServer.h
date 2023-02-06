#pragma once
#include <map>
#include <vector>
#include <string>
#include <sstream>
using namespace std;


#include <json/json.h>
#pragma hdrstop

#include "HTTPJsonResponse.h"
#include "HTTPTextResponse.h"
#include "HTTPResponse.h"
#include "TCPServer.h"

class ResponseDispatcher;
class CHTTPServer;

typedef struct _RequestHeader {
	string version;
	string method;
	string url;
	map<string, string> querystring;
	map<string, string> header;

	map<string, string> queryPayloads;
	Json::Value jsonPayloads;
} RequestHeader;


// V2 지원 콜백 함수 원형
typedef IHTTPResponse* (*fp_HTTPGetControllerV2)(CTCPServer& tcp, string uri, std::map<string, string> querystring, ResponseDispatcher& dispatcher);
typedef IHTTPResponse * (*fp_HTTPPostControllerV2)(CTCPServer& tcp, string uri, std::map<string, string> querystring, map<string, string> queryPayloads, Json::Value jsonPayloads, ResponseDispatcher& dispatcher);

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
	/**
	 * Response Header 정보에 Content-Type을 application/json 타입으로 응답하는 함수.
	 * 
	 * \param statusCode - 상태코드
	 * \param json - JSON 객체
	 * \param cors - CORS 정보, EX) *, http://~
	 * 
	 * \return 
	 */
	IHTTPResponse* JSON(int statusCode, Json::Value json, string cors = "")
	{
		response = new CHTTPJsonResponse(json);
		response->SetCORS(cors);
		response->SetStatusCode(statusCode);
		return response;
	}

	/**
	 * Response Header 정보에 Content-Type을 text/html 타입으로 응답하는 함수..
	 * 
	 * \param statusCode - 상태코드
	 * \param json - string 객체
	 * \param cors - CORS 정보, EX) *, http://~
	 * \return 
	 */
	IHTTPResponse* Text(int statusCode, string text, string cors = "")
	{
		response = new CHTTPTextResponse(text);
		response->SetCORS(cors);
		response->SetStatusCode(statusCode);
		return response;
	}

private:
	IHTTPResponse * response;
	
};

class CHTTPServer : public IServer
{
public:
	CHTTPServer();
public:
	// V1 Spec
	void SetController(fp_HTTPGetController		fp);
	void SetController(fp_HTTPPostController	fp);

	// V2 Sepc
	void SetController(fp_HTTPGetControllerV2	fp);
	void SetController(fp_HTTPPostControllerV2	fp);

public:
	bool Receive(CTCPServer& tcp);
	bool ReceiveV2(CTCPServer& tcp);

private:
	string TcpCallbackSelectorFromV1(CTCPServer& tcp);
	IHTTPResponse* TcpCallbackSelectorFromV2(CTCPServer& tcp);

private:
	bool Parse(const string& data, RequestHeader& ref);
	bool Split(vector<string>& ref, string src, std::string delimeter);
	bool isHttp();
	

	fp_HTTPGetController	fp_GetController;
	fp_HTTPPostController	fp_PostController;

	fp_HTTPGetControllerV2	fp_GetControllerV2;
	fp_HTTPPostControllerV2	fp_PostControllerV2;

private:
	SOCKET m_client; // 현재 소켓
	RequestHeader header; // 헤더

private:
	ResponseDispatcher dispatcher;
};

