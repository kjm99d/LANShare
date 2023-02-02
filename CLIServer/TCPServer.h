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

using namespace std;
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
	void SendTo(const CLIENT_INFOMATION& info);
	void SendAll(const char* src, const char* file_name);
	void HeartBeat(string & responseBody);

private:
	// 클라이언트 스토리지
	vector<CLIENT_INFOMATION> clients;
};

