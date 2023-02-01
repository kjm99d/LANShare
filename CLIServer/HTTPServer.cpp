#include "HTTPServer.h"

void CHTTPServer::Receive(fp_HTTPEvent fp_callback)
{
	SOCKET sock;
	if (true == Accept(sock))
	{
		unsigned char buffer[4096] = { 0, };
		const int buffer_size = sizeof(buffer) / sizeof(char);


		string packet;
		int recv_size = -1;
		while (recv_size == -1)
		{
			recv_size = recv(sock, (char*)buffer, buffer_size, 0);
 		}
		
		if (recv_size != SOCKET_ERROR)
		{
			packet.append((char*)buffer, recv_size);

#if 0

			while ((recv_size = recv(sock, (char*)buffer, buffer_size, 0)))
			{
				if (recv_size == SOCKET_ERROR)
				{
					if (WSAGetLastError() != WSAEWOULDBLOCK) {
						//printf("No Recved Data")
						break;
					}


				}

				if (recv_size == -1)
					break;
				packet.append((char*)buffer, recv_size);
		}
#endif
			RequestHeader request_header;
			Parse(packet, request_header);


			const char* str_header = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nContent-length: 3\r\n\r\n";
			const char* str_body = "aaa\r\n";

			while (send(sock, str_header, strlen(str_header), 0) == -1);
			while (send(sock, str_body, strlen(str_body), 0) == -1);

			closesocket(sock);
			//sock = nullptr;

			if (nullptr != fp_callback);
			//fp_callback(sock, (unsigned char *)packet.c_str(), packet.size());
		}
		else
		{
			printf("Sock = [%d] \n", recv_size);
		}
	}
}

bool CHTTPServer::Parse(const string& data,  RequestHeader& ref)
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
			ref.header.insert({tokens[0], tokens[1]});
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

