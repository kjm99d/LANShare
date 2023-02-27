#pragma once
#include <map>
#include <vector>
#include <string>
#include <sstream>
//using namespace std;


#include <json/json.h>
#pragma hdrstop

#include "HTTPJsonResponse.h"
#include "HTTPTextResponse.h"
#include "HTTPResponse.h"
#include "TCPServer.h"
#include "ResponseJsonProvider.h"

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
typedef IHTTPResponse * (*fp_HTTPGetControllerV2)(CTCPServer& tcp, std::string uri, std::map<string, string> querystring, ResponseDispatcher& dispatcher);
typedef IHTTPResponse * (*fp_HTTPPostControllerV2)(CTCPServer& tcp, std::string uri, std::map<string, string> querystring, map<string, string> queryPayloads, Json::Value jsonPayloads, ResponseDispatcher& dispatcher);

// V1 지원 콜백 함수 원형
typedef std::string (*fp_HTTPGetController)(CTCPServer& tcp, std::string uri, std::map<string, string> querystring);
typedef std::string (*fp_HTTPPostController)(CTCPServer& tcp, std::string uri, std::map<string, string> querystring, map<string, string> queryPayloads, Json::Value jsonPayloads);

/**
 * 콜백 컨트롤러에서 만큼은 지원하는 범위 내에서 입맛대로 호출하세요.
 */
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
	IHTTPResponse* JSON(int statusCode, Json::Value json, std::string cors = "")
	{
		response = new CHTTPJsonResponse(json);
		response->SetCORS(cors);
		response->SetStatusCode(statusCode);
		return response;
	}

	/**
	 * CResponseJsonProvider 포맷으로 응답하는 함수.
	 * 
	 * \param statusCode - 상태코드
	 * \param message - 상태코드에 따른 메세지 정보
	 * \param json - JSON 객체
	 * \param cors - CORS 정보
	 * \return 
	 */
	IHTTPResponse* JSON(int statusCode, std::string message, Json::Value json, std::string cors = "")
	{
		response = new CResponseJsonProvider(statusCode, message, json);
		response->SetCORS(cors);
		response->SetStatusCode(200);
		return response;
	}

	/**
	 * Response Header 정보에 Content-Type을 text/html 타입으로 응답하는 함수..
	 * 
	 * \param statusCode - 상태코드
	 * \param json - std::string 객체
	 * \param cors - CORS 정보, EX) *, http://~
	 * \return 
	 */
	IHTTPResponse* Text(int statusCode, std::string text, std::string cors = "")
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
	void ClearRequest();

private:
	string TcpCallbackSelectorFromV1(CTCPServer& tcp);
	IHTTPResponse* TcpCallbackSelectorFromV2(CTCPServer& tcp);

private:
	bool Parse(const string& data, RequestHeader& ref);
	bool Split(vector<string>& ref, std::string src, std::string delimeter);
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

