

#include "NBSocketServer.h"

#include <vector>
#include <signal.h>
#include <iostream>
#include <thread>


#include "INISettingLoader.h"
#include "ProtocolBase.h"
#include "ProtocolV1.h"


// ==============
string fileName;
string fhash;
UINT fsize;
// ==============



void     INThandler(int);
static volatile int keepRunning = 1;



void RecvPacket(std::vector< std::pair<SOCKET, SOCKADDR_IN>>& sockets);
BOOL AddClient(std::vector< std::pair<SOCKET, SOCKADDR_IN>>& sockets, SOCKET& sock_tcp_listen);
void PrintCommand();
void PrintClient(const std::vector<std::pair<SOCKET, SOCKADDR_IN>> sockets);
int LoadINI();

std::vector< std::pair<SOCKET, SOCKADDR_IN>> sockets;

int main(int argc, const char* argv[])
{
	// [Ctrl + C] Interrupt
	signal(SIGINT, INThandler);


	const long nPort = 5003;
	CNBSocketServer server;
	server.SetPort(nPort);
	server.Bind();



	std::thread CommandListener = std::thread([] {
		// 명령어 입력 받기
		while (true) {
			system("cls"); // 콘솔창 지워라 
			PrintCommand();
		}
		});

	

	// ===============================================================================
	// Connection Co-Routine
	// ===============================================================================
	int counter = 0;
	
	
	
	while (keepRunning)
	{ 
		// 패킷을 수신한다
		SOCKET Sock = server.GetListen();
		RecvPacket(sockets);
		AddClient(sockets, Sock);

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
		//printf("TCP - %s:%d] %s \n", inet_ntoa(sockets[i].second.sin_addr), ntohs(sockets[i].second.sin_port), buf);
		
		CProtocolV1* protocol = new CProtocolV1();
		protocol->SetMessage(buf);
		if (protocol->Parse() == true)
			protocol->Start();



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
	printf("| [3] 현재 연결된 모든 클라이언트 정보 출력	                 | \n");
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
		LoadINI(); break;
	case 3:
		PrintClient(sockets); break;
	default:
		break;
	}

	system("pause");
}

void PrintClient(const std::vector<std::pair<SOCKET, SOCKADDR_IN>> sockets)
{
	for(int i = 0; i < sockets.size(); ++i)
	{
		const int num = i + 1;
		const std::pair<SOCKET, SOCKADDR_IN> client = sockets[i];
		printf("[%d] 클라이언트 접속 : IP 주소 = %s\t 포트 번호 = %d\n", num, inet_ntoa(client.second.sin_addr), ntohs(client.second.sin_port));

	}
}


int LoadINI()
{
	std::string temp; // 임시 변수

	CINISettingLoader ldr(".\\test.ini");
	ldr.SetSection("Profile");
	ldr.Get("FileName", fileName);
	ldr.Get("hash", fhash);
	//ldr.Get("size", fsize);
	ldr.Get("size", fsize);



	return 0;
}


void  INThandler(int sig)
{
	signal(sig, SIG_IGN);
	printf("break\n");
	keepRunning = 0;
}
