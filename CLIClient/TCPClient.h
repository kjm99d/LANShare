#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#pragma comment (lib , "ws2_32.lib")

#include <string>



#pragma hdrstop

#define ERR_TCP_INVALID_SOCKET  0x01
#define ERR_TCP_CONNECT_ERROR	0x02

class CTCPClient
{
public:
	CTCPClient() = delete;
	CTCPClient(std::string address, int port);
	virtual ~CTCPClient();

public:
	SOCKET GetSock();
	int Connect();
	int Close();

private:
	void SetNonBlocking();


private:

	void SafeSend(char* buffer, int length);
	bool SafeRecv(std::string& ref_buffer);

private:
	std::string m_address;
	int m_port;

	SOCKET m_socket;
};

