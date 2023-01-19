

#include "NBSocketServer.h"

#include <vector>
#include <signal.h>
#include <iostream>


#include "INISettingLoader.h"

void     INThandler(int);

static volatile int keepRunning = 1;



void RecvPacket(std::vector< std::pair<SOCKET, SOCKADDR_IN>>& sockets);
BOOL AddClient(std::vector< std::pair<SOCKET, SOCKADDR_IN>>& sockets, SOCKET& sock_tcp_listen);
void PrintCommand();
int LoadINI();

int main(int argc, const char* argv[])
{
	signal(SIGINT, INThandler);


	const long nPort = 5003;
	CNBSocketServer server;
	server.SetPort(nPort);
	server.Bind();


	// ===============================================================================
	// 명령어 입력 받기
	PrintCommand();
	// ===============================================================================
	

	// ===============================================================================
	// Connection Co-Routine
	// ===============================================================================
	
	std::vector< std::pair<SOCKET, SOCKADDR_IN>> sockets;
	
	int counter = 0;



	while (keepRunning)
	{ // 프로그램 대기상태를 위한 #while_1
		// 패킷을 수신한다
		SOCKET Sock = server.GetListen();
		RecvPacket(sockets);
		AddClient(sockets, Sock);


		if (sockets.size() > 0 && 0 == counter) 
		{
			std::string path;
			std::cout << "PATH >> " << std::endl;
			std::cin >> path;

			FILE* fd = NULL;
			fopen_s(&fd, path.c_str(), "rb");

			fseek(fd, 0, SEEK_END);
			long length = ftell(fd);
			fseek(fd, 0, SEEK_SET);

			while (true) 
			{
				unsigned char buffer[1024] = { 0, };
				const size_t read_byte = fread(buffer, 1, 1024, fd);
				if (read_byte == 0) 
				{
					break;
				}

				send(sockets[0].first, (const char*)buffer, read_byte, 0);
			}

			fclose(fd);
		}

#if 0
		counter++;
		 if (counter % 10000 == 0) printf("counter = [%d] \n", counter);
		if (counter == 100000)
		{
			counter = 0;
			// Send 기능 테스트
			for (int i = 0; i < sockets.size(); ++i)
			{
				const char* arg = "hello world     ";
				send(sockets[i].first, arg, strlen(arg), 0);
			}
		}
#endif

	} // END #while_1

	// ===============================================================================
	server.Clear();

	return 0;
}


void RecvPacket(std::vector< std::pair<SOCKET, SOCKADDR_IN>>& sockets)
{
	int ret_value = 0;
	char buf[BUFSIZE + sizeof(TCHAR)];

	// 메세지 목록을 먼저 본다.
	unsigned int i;
	for (i = 0; i < sockets.size(); ++i)
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
			//sockets.erase(sockets.begin(), i);
			sockets.erase(sockets.begin() + i);
			if (i > 0) --i;
			continue;
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



void PrintCommand()
{
	COORD Cur;
	CONSOLE_SCREEN_BUFFER_INFO a;
	int menu_num = 0;


	printf("| ---------------------------------------------------------- | \n");
	printf("| ---------------------- Command Box ----------------------- | \n");
	printf("| ---------------------------------------------------------- | \n");
	printf("| [1] 파일로 부터 읽기(base.ini)                             | \n");
	printf("| [2] 커맨드로 읽기                                          | \n");
	printf("| ---------------------------------------------------------- | \n");
	printf("| COMMAND >> ");
	// 현재 커서 정보를 가져온다
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &a);
	printf("                                                |");
	Cur.X = a.dwCursorPosition.X;
	Cur.Y = a.dwCursorPosition.Y;
	printf("\n"); // newline
	printf("| ---------------------------------------------------------- | \n");
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);

	scanf_s("%d", &menu_num);
	Cur.X = 0;
	Cur.Y += 2;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);


	switch (menu_num)
	{
	case 1:
		LoadINI();
		break;
	default:
		break;
	}
}

int LoadINI()
{
	std::string temp; // 임시 변수

	CINISettingLoader ldr("test.ini");
	ldr.SetSection("Profile");
	ldr.Get("FileName", temp);
	printf("%s", temp.c_str());



	return 0;
}


void  INThandler(int sig)
{
	signal(sig, SIG_IGN);
	printf("break\n");
	keepRunning = 0;
}
