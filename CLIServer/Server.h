#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <string>
#include <Windows.h>

using namespace std;

#ifdef SetPort
#undef SetPort
#endif


#ifdef UNICODE
#undef UNICODE
#endif

#ifdef _UNICODE
#undef _UNICODE
#endif


#pragma comment(lib, "ws2_32.lib")




class IServer
{
public:
	IServer();
	virtual ~IServer();

	void SetPort(long port);
	BOOL Bind();
	void Clear();

	SOCKET GetListen() { return listen_sock; };

protected:
	bool Accept(SOCKET& ref_socket);
	bool Accept(SOCKET& ref_socket, SOCKADDR_IN& ref_addr_in);
	void SafeSend(SOCKET& ref_sock, char* buffer, int length);
	bool SafeRecv(SOCKET& ref_sock, std::string& ref_buffer);

private:
	long port;
	SOCKET listen_sock;
};

