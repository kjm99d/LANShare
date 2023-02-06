#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#pragma comment (lib , "ws2_32.lib")


class CTCPClient
{
public:
	CTCPClient();
	~CTCPClient();

	int Connect();
	int Close();


private:
	SOCKET socket;
};

