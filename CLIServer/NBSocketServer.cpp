#include "NBSocketServer.h"

// ================================================== //
CNBSocketServer::CNBSocketServer() : port(0), listen_sock(0)
{
}

CNBSocketServer::~CNBSocketServer()
{
}

void CNBSocketServer::SetPort(long port)
{
	this->port = port;
}

BOOL CNBSocketServer::Bind()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		//printf("WSAStartup Failure \n");
		return FALSE;
	}

	// 리턴값을 저장시킬 변수
	int ret_value = 0;

	// socket() - tcp 소켓 생성
	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) {
		//printf("Create TCP Socket Failure \n");
		return FALSE;
	}

	// 논블로킹 소켓으로 전환
	u_long sock_on = 1;
	ret_value = ioctlsocket(listen_sock, FIONBIO, &sock_on);
	if (ret_value == SOCKET_ERROR) {
		//printf("ioctlsocket() Failure \n");
		return FALSE;
	}

	// SO_REUSEADDR 소켓 옵션 설정(REUSEADDR)
	BOOL optReuse = TRUE;
	ret_value = setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&optReuse, sizeof(optReuse));
	if (ret_value == SOCKET_ERROR) {
		//printf("setsockopt() REUSEADDR Failure \n");
	}

	// bind() - 서버의 지역 IP주소와 지역포트 번호를 위한 구조체 전달
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr.S_un.S_addr = htons(INADDR_ANY);
	ret_value = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (ret_value == SOCKET_ERROR) {
		//printf("bind() Failure \n");
		return FALSE;
	}


	// listen() - bind에 의해 처리된 tcp 포트의 상태정보를 LISTEN으로 바꾼다.
	ret_value = listen(listen_sock, SOMAXCONN);
	if (ret_value == SOCKET_ERROR) {
		//printf("listen() Failure \n");
		return FALSE;
	}

	return TRUE;
}

void CNBSocketServer::Clear()
{
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
}

// ================================================== //







