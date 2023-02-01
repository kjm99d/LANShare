#pragma once
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

#include "Server.h"

typedef int (*fp_TCPEvent)(SOCKET sock);

class CTCPServer : public IServer
{
public:
	CTCPServer();
	~CTCPServer();

public:
	int Receive(fp_TCPEvent cb_callback);


private:
	vector<SOCKET> clients;
};

