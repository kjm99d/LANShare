#include "HTTPServer.h"

void CHTTPServer::Receive(fp_HTTPEvent fp_callback)
{
	SOCKET sock;
	if (true == Accept(sock))
	{	
		

		unsigned char buffer[4096] = { 0, };
		int buffer_size = sizeof(buffer) / sizeof(char);

		int recv_size = 0;
		string packet;
		while ((recv_size = recv(sock, (char*)buffer, buffer_size, 0))) 
		{
			if (recv_size == -1) 
				break;
			packet.append((char*)buffer, recv_size);
		}
		RequestHeader request_header;
		Parse(packet, request_header);
		
		if (nullptr != fp_callback);
			//fp_callback(sock, (unsigned char *)packet.c_str(), packet.size());
	}
}

bool CHTTPServer::Parse(const string& data,  RequestHeader& ref)
{
	// http 아님
	if (data.find("\r\n\r\n") == string::npos)
		return false;


	vector<string> lines;
	Split(lines, data, "\r\n");
	for (int i = 1; i < lines.size(); ++i)
	{
		vector<string> tokens;
		if (Split(tokens, lines[i], ": "))
		{

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

