#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#pragma comment (lib , "ws2_32.lib")

#include <vector>
#include <string>
#include <fstream>
#include <stdint.h>
#include <ProtocolID.h>

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
	// 임시
	SOCKET m_socket;

	int Connect();
	int Close();
	void ReConnect();

	void Receive();
	int ReadHeader(uintmax_t& ref_length);

private:
	void SetNonBlocking();


private:
	void SafeSend(char* buffer, int length);
	bool SafeRecv(std::string& ref_buffer);

private:
	std::string m_address;
	int m_port;

};

