﻿#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>

#include <Windows.h>

#ifdef SetPort
#undef SetPort
#endif


#ifdef UNICODE
#undef UNICODE
#endif

#ifdef _UNICODE
#undef _UNICODE
#endif

#define BUFSIZE 512


#pragma comment(lib, "ws2_32.lib")

#include <vector>
#pragma hdrstop

#include "FileReader.h"
#include "ProtocolID.h"
#include "BufferWriter.h"
#include "CommandGenerater.h"

#include "Server.h"

//typedef int (*fp_TCPEvent)(SOCKET sock);
typedef int (*fp_TCPEvent)(SOCKET sock);

typedef enum {
	kNone = 0x00,
	kSendTo,
	kSendAll,
} eCallbackHeader;



typedef struct _CLIENT_INFOMATION {
	SOCKET SOCK;
	SOCKADDR_IN ADDR;

	std::string address() { return inet_ntoa(ADDR.sin_addr); }
	u_short port() { return ntohs(ADDR.sin_port); }

} CLIENT_INFOMATION;

class CTCPServer : public IServer
{
public:
	CTCPServer();
	~CTCPServer();

public:
	void ShowClient();
	void AddClient();
	int Receive(fp_TCPEvent cb_callback);


public:
	void SendTo(std::string address, const char* src, const char* file_name);
	void SendAll(const char* src, const char* file_name);
	void HeartBeat(std::string & reponsebody);
	void Echo(std::string msg, vector<CLIENT_INFOMATION> targets = {});
private:
	bool FindClientFromAddress(std::string address, CLIENT_INFOMATION& client);

private:
	// 클라이언트 스토리지
	vector<CLIENT_INFOMATION> clients;
};

