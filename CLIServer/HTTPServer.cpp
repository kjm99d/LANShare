#include "HTTPServer.h"

CHTTPServer::CHTTPServer()
{
	fp_GetController = nullptr;
	fp_PostController = nullptr;

	fp_GetControllerV2 = nullptr;
	fp_PostControllerV2 = nullptr;

}

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

void CHTTPServer::SetController(fp_HTTPPostControllerV2 fp)
{
	this->fp_PostControllerV2 = fp;
}


bool CHTTPServer::Receive(CTCPServer& tcp)
{

	bool ret = false;

	if (isHttp())
	{
		const char* str_header = "HTTP/1.1 200 OK\r\n";
		// CORS 헤더
		const char* str_header2 = "Access-Control-Allow-Headers: *\r\n";
		const char* str_header3 = "Access-Control-Allow-Origin: *\r\n\r\n";
		const char* str_body = "aaa";


		string strResponseBody = TcpCallbackSelectorFromV1(tcp);
		if (strResponseBody.size() > 0)
		{
			SafeSend(m_client, (char*)str_header, strlen(str_header));
			SafeSend(m_client, (char*)str_header2, strlen(str_header2));
			SafeSend(m_client, (char*)str_header3, strlen(str_header3));
			SafeSend(m_client, (char*)strResponseBody.c_str(), strResponseBody.size());

			closesocket(m_client);

			ret = true;
		}
	}

	return ret;

}

bool CHTTPServer::ReceiveV2(CTCPServer& tcp)
{
	bool ret = false;

	if (isHttp())
	{
		IHTTPResponse* response = TcpCallbackSelectorFromV2(tcp);
		if (response)
		{
			response->SetHttpVersion(header.version);

			ResponseDispatcher dispatcher;
			if (nullptr != response)
			{
				string cors = response->GetCORSHeader();
				//const char* str_header = "HTTP/1.1 200 OK\r\n";
				SafeSend(m_client, (char*)response->GetStartLine().c_str(), response->GetStartLine().size());
				SafeSend(m_client, (char*)response->GetContentTypeHeader().c_str(), response->GetContentTypeHeader().size());
				SafeSend(m_client, (char*)response->GetCORSHeader().c_str(), response->GetCORSHeader().size());
				SafeSend(m_client, (char*)"\r\n", strlen("\r\n"));
				SafeSend(m_client, (char*)response->GetResponseBody().c_str(), response->GetResponseBody().size());
				closesocket(m_client);
				ret = true;
			}
		}
	}

	return ret;

}

string CHTTPServer::TcpCallbackSelectorFromV1(CTCPServer& tcp)
{
	string response;
	if (header.method.compare("GET") == 0 && fp_GetController != nullptr)
		response = fp_GetController(tcp,  header.url, header.querystring);
	else if (header.method.compare("POST") == 0 && fp_PostController != nullptr)
		response = fp_PostController(tcp,  header.url, header.querystring, header.queryPayloads, header.jsonPayloads);
	// else 404Page

	return response;
}

IHTTPResponse* CHTTPServer::TcpCallbackSelectorFromV2(CTCPServer& tcp)
{
	const string& method = header.method;
	const string& url = header.url;
	const map<string, string>& querystring = header.querystring;
	const map<string, string>& queryPayloads = header.queryPayloads;
	const Json::Value& jsonPayloads = header.jsonPayloads;
	
	

	IHTTPResponse* response = nullptr;
	if (method.compare("GET") == 0 && fp_GetControllerV2 != nullptr)
		response = fp_GetControllerV2(tcp, url, querystring, dispatcher);
	else if (method.compare("POST") == 0 && fp_PostControllerV2 != nullptr)
		response = fp_PostControllerV2(tcp, url, querystring, queryPayloads, jsonPayloads, dispatcher);
	// else 404Page

	return response;
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
		ref.version = tokens[2];
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

bool CHTTPServer::Split(vector<string>& ref, std::string src, std::string delimeter)
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

bool CHTTPServer::isHttp()
{
	bool ret = false;

	if (true == Accept(m_client))
	{
		string packet;
		if (true == SafeRecv(m_client, packet))
		{
			if (true == Parse(packet, header))
			{
				ret = true;
			}
		}
	}

	return ret;
}

