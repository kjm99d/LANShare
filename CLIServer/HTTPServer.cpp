﻿#include "HTTPServer.h"

void CHTTPServer::SetController(fp_HTTPGetController fp)
{
	this->fp_GetController = fp;
}

void CHTTPServer::SetController(fp_HTTPPostController fp)
{
	this->fp_PostController = fp;
}

void CHTTPServer::SetController(fp_HTTPGetControllerV2 fp)
{
	this->fp_GetControllerV2 = fp;
}

void CHTTPServer::SetCORS(string value)
{
	this->cors = value;
}

bool CHTTPServer::Receive(CTCPServer& tcp)
{

	bool ret = false;
	SOCKET sock;
	if (true == Accept(sock))
	{
		string packet;
		if (true == SafeRecv(sock, packet))
		{
			RequestHeader request_header;
			if (true == Parse(packet, request_header))
			{

				const char* str_header = "HTTP/1.1 200 OK\r\n";
				// CORS 헤더
				const char* str_header2 = "Access-Control-Allow-Headers: *\r\n";
				const char* str_header3 = "Access-Control-Allow-Origin: *\r\n\r\n";
				const char* str_body = "aaa";

				IHTTPResponse* response = nullptr;
				string strResponseBody;
				if (request_header.method.compare("GET") == 0 && fp_GetController != nullptr)
					strResponseBody = fp_GetController(tcp, sock, request_header.url, request_header.querystring);
				else if(request_header.method.compare("POST") == 0 && fp_PostController != nullptr)
					strResponseBody = fp_PostController(tcp, sock, request_header.url, request_header.querystring, request_header.queryPayloads, request_header.jsonPayloads);

				SafeSend(sock, (char*)str_header, strlen(str_header));
				SafeSend(sock, (char*)str_header2, strlen(str_header2));
				SafeSend(sock, (char*)str_header3, strlen(str_header3));
				SafeSend(sock, (char*)strResponseBody.c_str(), strResponseBody.size());

				closesocket(sock);

				ret = true;
			}
		}
	}

	return ret;

}

bool CHTTPServer::ReceiveV2(CTCPServer& tcp)
{

	bool ret = false;
	SOCKET sock;
	if (true == Accept(sock))
	{
		string packet;
		if (true == SafeRecv(sock, packet))
		{
			RequestHeader request_header;
			if (true == Parse(packet, request_header))
			{

				const char* str_header = "HTTP/1.1 200 OK\r\n";
				// CORS 헤더
				const char* str_header2 = "Access-Control-Allow-Headers: *\r\n";
				const char* str_header3 = "Access-Control-Allow-Origin: *\r\n\r\n";

				/*
				* ResponseDispatcher 를 만들어서 걔가 IHTTPResponse 구현체를 반환해주면 될 것 같다.
				*/
				// 여기로 받아와서 얘기 가져와서 다 뱉어주면 될 것 같음.
				IHTTPResponse* response = nullptr;
				ResponseDispatcher dispatcher; // 얘를 레퍼런스로 넘겨서 이 객체에서 특정함수로 반환해주게 해서 그걸 받아서 ... 응응 .. 얘가 지 알아서 소멸자 호출하게해서 메모리릭 안생기게 풍풍! 푱 !
				
				// (CTCPServer & tcp, SOCKET sock, string uri, std::map<string, string> querystring, ResponseDispatcher & dispatcher);
				if (request_header.method.compare("GET") == 0 && fp_GetControllerV2 != nullptr)
					response = fp_GetControllerV2(tcp, sock, request_header.url, request_header.querystring, dispatcher);
				//else if (request_header.method.compare("POST") == 0 && fp_PostController != nullptr)
				//	strResponseBody = fp_PostController(tcp, sock, request_header.url, request_header.querystring, request_header.queryPayloads, request_header.jsonPayloads);

				//SafeSend(sock, (char*)str_header, strlen(str_header));
				response->SetHttpVersion("HTTP/1.1");
				response->SetStatusCode(200);
				SafeSend(sock, (char*)response->GetStartLine().c_str(), response->GetStartLine().size());
				SafeSend(sock, (char*)str_header2, strlen(str_header2));
				SafeSend(sock, (char*)str_header3, strlen(str_header3));
				SafeSend(sock, (char*)response->GetResponseBody().c_str(), response->GetResponseBody().size());

				closesocket(sock);

				ret = true;
			}
		}
	}

	return ret;

}

bool CHTTPServer::Parse(const string& data, RequestHeader& ref)
{
	// HTTP 아님
	if (data.find("\r\n\r\n") == string::npos)
		return false;

	// HTTP 가능성이 있음
	vector<string> container;
	Split(container, data, "\r\n\r\n");
	if (container.size() != 2)
	{
		return false;
	}

	// Payloads Parse

	//ref.payloads = 

	// RequestHeader 영역 파싱
	vector<string> lines;
	Split(lines, container[0], "\r\n");

	// Start Line & QueryStrng Parse
	vector<string> tokens;
	Split(tokens, lines[0], " ");
	if (tokens.size() == 3)
	{
		ref.method = tokens[0];

		vector<string> vec, vec2;
		if (Split(vec, tokens[1], "?"))
		{
			ref.url = vec[0]; // URL
			Split(vec2, vec[1], "&");

			for (auto v : vec2)
			{
				vector<string> temp;
				Split(temp, v, "=");
				ref.querystring.insert({ temp[0], temp[1] });
			}
		}
		else
			ref.url = tokens[1];
	}
	else
	{
		return false;
	}

	// HTTP HeaderParse
	for (int i = 1; i < lines.size(); ++i)
	{
		vector<string> tokens;
		if (Split(tokens, lines[i], ": ")) 
			ref.header.insert({ tokens[0], tokens[1] });
	}

	string& payloads = container[1];
	if (ref.method.compare("GET") == 0)
	{
		// 얘는 쿼리스트링만 따면 됨 바디보내는거 비표준임.
	}
	else if (ref.method.compare("POST") == 0)
	{
		if (ref.header["Content-Type"].find("application/x-www-form-urlencoded") != string::npos)
		{
			vector<string> vec;
			Split(vec, payloads, "&");

			for (auto v : vec)
			{
				vector<string> temp;
				Split(temp, v, "=");
				ref.queryPayloads.insert({ temp[0], temp[1] });
			}
		}
		else if (ref.header["Content-Type"].find("application/json") != string::npos)
		{
			// HA.... 살려줘
			// const std::string rawJson = R"({"Age": 20, "Name": "colin"})";
#if 1
			std::string whitespaces("\r\n");
			while (true) {
				std::size_t found = payloads.find(whitespaces);
				if (found == std::string::npos)
					break;
				
				payloads.erase(found, 2);
			}
#endif
			const auto rawJsonLength = static_cast<int>(payloads.length());

			constexpr bool shouldUseOldWay = false;
			JSONCPP_STRING err;
			Json::Value root;

			if (shouldUseOldWay) {
				Json::Reader reader;
				reader.parse(payloads, root);
			}
			else {
				Json::CharReaderBuilder builder;
				const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
				if (!reader->parse(payloads.c_str(), payloads.c_str() + rawJsonLength, &ref.jsonPayloads,
					&err)) {
					
					
				}
			}
		}
	}
	
	return true;
}

bool CHTTPServer::Split(vector<string>& ref, string src, std::string delimeter)
{
#if 0
	stringstream stream(src);
	string token;
	while (std::getline(stream, token, delimeter))
		ref.push_back(token);

	const bool ret = ref.size() > 0;
	return ret;
#endif


	size_t pos = 0;
	std::string token;
	while ((pos = src.find(delimeter)) != std::string::npos) {
		token = src.substr(0, pos);
		if (token.size() > 0) ref.push_back(token);
		src.erase(0, pos + delimeter.length());
	}
	if (token.size() > 0) ref.push_back(src);

	const bool ret = ref.size() > 0;
	return ret;
}

