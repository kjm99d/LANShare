#include "HTTPServer.h"

bool CHTTPServer::Receive(fp_HTTPEvent fp_callback)
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

				const char* str_header = "HTTP/1.0 200 OK\r\n\r\n";
				const char* str_body = "aaa";

				string res;
				if (nullptr != fp_callback);
					fp_callback(sock, request_header.method, request_header.url, res);

				SafeSend(sock, (char*)str_header, strlen(str_header));
				SafeSend(sock, (char*)res.c_str(), res.size());

				closesocket(sock);

				ret = true;
			}
		}
	}

	return ret;

}

bool CHTTPServer::Parse(const string& data, RequestHeader& ref)
{
	// http 아님
	if (data.find("\r\n\r\n") == string::npos)
		return false;

	vector<string> container;
	Split(container, data, "\r\n\r\n");
	if (container.size() != 2)
	{
		return false;
	}

	ref.payloads = container[1];

	// RequestHeader 영역 파싱
	vector<string> lines;
	Split(lines, container[0], "\r\n");

	// 요청 기본 정보 파싱
	vector<string> tokens;
	Split(tokens, lines[0], " ");
	if (tokens.size() == 3)
	{
		ref.method = tokens[0];
		ref.url = tokens[1];
	}
	else
	{
		return false;
	}

	for (int i = 1; i < lines.size(); ++i)
	{
		vector<string> tokens;
		if (Split(tokens, lines[i], ": "))
			ref.header.insert({ tokens[0], tokens[1] });
	}


	// RequestBody 영역 파싱


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

