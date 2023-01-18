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

//typedef int (*ProtocolV1)(BYTE packet, int length);
//typedef int (*ProtocolV2)(BYTE packet, int length);



class CNBSocketServer
{
public:
	CNBSocketServer();
	~CNBSocketServer();

	void SetPort(long port);
	BOOL Bind();
	void Clear();
	SOCKET GetListen() { return listen_sock; };


private:
	long port;
	SOCKET listen_sock;
};

