#include "HTTPServer.h"

void CHTTPServer::Receive(fp_HTTPEvent fp_callback)
{
	SOCKET sock;
	if (true == Accept(sock))
	{
		unsigned char buffer[100] = { 0, };
		const int buffer_size = sizeof(buffer) / sizeof(char);


		string packet;
		while (true)
		{
			const int recv_size = recv(sock, (char*)buffer, buffer_size, 0);
			const int err = WSAGetLastError();
			//printf("Receive Size = [%d] LastError = [%d] \n", recv_size, err);

			if (buffer_size > recv_size && 0 == err)
				break;
			if (WSAEWOULDBLOCK == err)
				continue;

#if 0
			if (0 == err)
			{
				packet.append((char*)buffer, recv_size);
			}
			else if (WSAEWOULDBLOCK == err)
			{
				if (0 < packet.size())
					break;
				else if (SOCKET_ERROR == recv_size)
					continue;
			}	
#endif
		}

		
		RequestHeader request_header;
		Parse(packet, request_header);

		const char* str_header = "HTTP/1.0 200 OK\r\n\r\n";
		const char* str_body = "aaa";

		while (send(sock, str_header, strlen(str_header), 0) == -1);
		while (send(sock, str_body, strlen(str_body), 0) == -1);

		closesocket(sock);
		//sock = nullptr;

		if (nullptr != fp_callback);
		//fp_callback(sock, (unsigned char *)packet.c_str(), packet.size());
	}

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

