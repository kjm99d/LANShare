#ifdef UNICODE
	#undef UNICODE
#endif

#ifdef _UNICODE
	#undef _UNICODE
#endif

#define BUFSIZE 512

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <stdlib.h>

#include <vector>

void RecvPacket(std::vector< std::pair<SOCKET, SOCKADDR_IN>>& sockets);
BOOL AddClient(std::vector< std::pair<SOCKET, SOCKADDR_IN>>& sockets, SOCKET& sock_tcp_listen);

int main(int argc, const char* argv[])
{
	const long nPort = 5003;

	// 윈속 초기화에 실패할 경우 종료
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("WSAStartup Failure \n");
		return -1;
	}

	// 리턴값을 저장시킬 변수
	int ret_value = 0;

	// socket() - tcp 소켓 생성
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) {
		printf("Create TCP Socket Failure \n");
		return -1;
	}

	// 논블로킹 소켓으로 전환
	u_long sock_on = 1;
	ret_value = ioctlsocket(listen_sock, FIONBIO, &sock_on);
	if (ret_value == SOCKET_ERROR) {
		printf("ioctlsocket() Failure \n");
		return -1;
	}

	// SO_REUSEADDR 소켓 옵션 설정(REUSEADDR)
	BOOL optReuse = TRUE;
	ret_value = setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&optReuse, sizeof(optReuse));
	if (ret_value == SOCKET_ERROR) {
		printf("setsockopt() REUSEADDR Failure \n");
	}

	// bind() - 서버의 지역 IP주소와 지역포트 번호를 위한 구조체 전달
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(nPort);
	serveraddr.sin_addr.S_un.S_addr = htons(INADDR_ANY);
	ret_value = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (ret_value == SOCKET_ERROR) {
		printf("bind() Failure \n");
		return -1;
	}


	// listen() - bind에 의해 처리된 tcp 포트의 상태정보를 LISTEN으로 바꾼다.
	ret_value = listen(listen_sock, SOMAXCONN);
	if (ret_value == SOCKET_ERROR) {
		printf("listen() Failure \n");
		return -1;
	}


	// ===============================================================================
	// Connection Co-Routine
	// ===============================================================================
	
	std::vector< std::pair<SOCKET, SOCKADDR_IN>> sockets;
	

	while (true) 
	{ // 프로그램 대기상태를 위한 #while_1
		
		// 패킷을 수신한다
		RecvPacket(sockets);
		AddClient(sockets, listen_sock);




	} // END #while_1

	// ===============================================================================

	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();

	return 0;
}


void RecvPacket(std::vector< std::pair<SOCKET, SOCKADDR_IN>>& sockets)
{
	int ret_value = 0;
	char buf[BUFSIZE + sizeof(TCHAR)];

	// 메세지 목록을 먼저 본다.
	for (int i = 0; i < sockets.size(); ++i)
	{ // 클라이언트와의 데이터 통신을 위한 #while_2
		ret_value = recv(sockets[i].first, buf, BUFSIZE, 0);
		if (ret_value == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				//printf("No Recved Data")
				break;
			}

			continue; // 음... 
		}
		else if (ret_value == 0)
		{
			// Peer Closed
			break;
		}

		buf[ret_value] = '\0';
		printf("TCP - %s:%d] %s \n", inet_ntoa(sockets[i].second.sin_addr), ntohs(sockets[i].second.sin_port), buf);

	} // END #while_2
}

BOOL AddClient(std::vector<std::pair<SOCKET, SOCKADDR_IN>>& sockets, SOCKET &sock_tcp_listen)
{
	int addrlen;

	addrlen = sizeof(SOCKADDR_IN);
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	// accept() - 서버에 접속한 클라이언트와 통신할 수 있도록 새로운 소켓을 생성리턴한다.
	client_sock = accept(sock_tcp_listen, (SOCKADDR*)&clientaddr, &addrlen);
	//printf("%d \n", var++);

	if (client_sock == INVALID_SOCKET) {
		//printf(">> [!] Accept() Failure \n");
		return FALSE;
	}

	sockets.push_back({ client_sock, clientaddr });

	printf("[TCP 서버] 클라이언트 접속 : IP 주소 = %s\t 포트 번호 = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return TRUE;
}
