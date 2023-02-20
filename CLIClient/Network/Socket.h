/*****************************************************************//**
 * \file   Socket.h
 * \brief  단순히 소켓연결과 보내기, 받기만 하는 클래스
 * 
 * \author KJM
 * \date   February 2023
 *********************************************************************/

#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#pragma comment (lib , "ws2_32.lib")

#include <string>


class CSocket
{
public:
	CSocket(int port, std::string address);
	~CSocket();

protected:
	int Connect();
	int Disconnect();


protected:
	bool SafeSend(const std::string& ref_buffer);
	bool SafeRecv(const std::string& ref_buffer);


protected:
	SOCKET m_socket;


private:
	int m_port;
	std::string m_address;


};

