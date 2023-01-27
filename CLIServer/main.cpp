

#include "NBSocketServer.h"

#include <vector>
#include <signal.h>
#include <iostream>
#include <thread>


#include "INISettingLoader.h"
#include "ProtocolBase.h"
#include "ProtocolV1.h"

#include "ProtocolID.h"			// 패킷에 정의될 프로토콜 ID
#include "CLIPacketStruct.h"	// 패킷정의 할 구조체 정보
#include "MemoryPool.h"
#include "MemoryStreamPool.h"
#include "CommandGenerater.h"

#include <string>
#include <stdlib.h>


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

BOOL GetCreateFilePacket(string path, PACKET_CreateFile& packet);
BOOL GetCloseHandlePacket(string path, PACKET_CloseHandle& packet);

std::vector< std::pair<SOCKET, SOCKADDR_IN>> sockets;

int main(int argc, const char* argv[])
{
	printf("%d", sizeof(PACKET_STREAM));
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
		
		CProtocolBase * protocol = new CProtocolV1();
		protocol->SetMessage(buf);
		protocol->SetClient(sockets[i].first);
		if (protocol->Parse() == true)
		{
			protocol->Start();
			closesocket(sockets[i].first);
		}

		// 메모리 해제
		delete protocol;
		protocol = nullptr;


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

	// Selector >> 나중에 함수로 별도로 만들어 둘 것
	// 입출력 부분과 이벤트 트리거 해주는 부분은 달라야함.
	switch (menu_num)
	{
	case 1:

		Cur.X = 0;
		Cur.Y += 2;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
		LoadINI(); break;
		
	case 3:
		PrintClient(sockets); 
		break;
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

	CINISettingLoader ldr("C:\\works\\LANShare\\bin\\x64\\Debug\\test.ini");
	ldr.SetSection("Profile");

	string dst;
	//ldr.Get("dst", dst);

	// ================================================================================================== //
	
	dst = "D:\\jjin.zip";
	CCommandGenerater cmdGen(PROTOCOL_ID_CREATEFILE, (char *)dst.c_str(), (int)dst.size() );
	cmdGen.Run();
	const char* bf = cmdGen.GetBuffer();
	int ssz = cmdGen.GetSize();
	for (int i = 0; i < sockets.size(); ++i)
		send(sockets[i].first, bf, ssz, 0);

	FILE* fd = nullptr;
	fopen_s(&fd, "D:\\test\\a.zip", "rb");
	char tempBuffer[256] = { 0, };

	
	while (true) {
		memset(tempBuffer, 0x00, 256);
		size_t read_size = fread(tempBuffer, 1, 256, fd);
		if (read_size == -1 || read_size == 0) break;
		cmdGen.SetCommand(PROTOCOL_ID_WRITEFILE);
		cmdGen.SetBuffer(tempBuffer, read_size);
		cmdGen.Run();
		const char* tempBuffer = cmdGen.GetBuffer();
		int sz = cmdGen.GetSize();
		printf(">> %d \n", sz);

		for (int i = 0; i < sockets.size(); ++i)
			send(sockets[i].first, tempBuffer, sz, 0);
		//Sleep(100);
	}
	fclose(fd); fd = nullptr;

	cmdGen.SetCommand(PROTOCOL_ID_CLOSEHANDLE);
	cmdGen.SetBufferPosition(0);
	cmdGen.Run();

	int tempSize = cmdGen.GetSize();
	for (int i = 0; i < sockets.size(); ++i)
		send(sockets[i].first, cmdGen.GetBuffer(), cmdGen.GetSize(), 0);

	// ================================================================================================== //

#if 0


	PACKET_STREAM stream;
	stream.cmd = 1;
	memcpy(stream.buffer + sizeof(int), "Hello WWWWW", sizeof("Hello WWWWW"));
	for (int i = 0; i < sockets.size(); ++i)
	{
		const char* buf = (char*)&stream.cmd;
		send(sockets[i].first, buf, sizeof(stream), 0);
	}

	PACKET_CreateFile packet;
	GetCreateFilePacket(dst, packet);
	for (int i = 0; i < sockets.size(); ++i)
	{
		const char* buf = (char*)&packet.cmd;
		send(sockets[i].first, buf, sizeof(packet), 0);
	}


	PACKET_WriteFile wf;
	wf.cmd = PROTOCOL_ID_WRITEFILE;

	// 전송
	memcpy(wf.buffer, "Hello World", sizeof("Hello World"));
	for (int i = 0; i < sockets.size(); ++i)
	{
		const char* buf = (char*)&wf.cmd;
		//							동적 메모리 이므로, 시작 주소부터 데이터의 끝 주소까지를 최종 버퍼사이즈로 잡아서 전송한다
		//const size_t buffer_size = sizeof(wf.cmd) + (512 * sizeof(char));
		send(sockets[i].first, buf, sizeof(wf), 0);
	}



	PACKET_CloseHandle packet2;
	GetCloseHandlePacket(dst, packet2);
	for (int i = 0; i < sockets.size(); ++i)
	{
		const char* buf = (char*)&packet2.cmd;
		send(sockets[i].first, buf, sizeof(packet2), 0);
	}

#endif
	return 0;
}


void  INThandler(int sig)
{
	signal(sig, SIG_IGN);
	printf("break\n");
	keepRunning = 0;
}





BOOL GetCreateFilePacket(string path, PACKET_CreateFile& packet)
{
	printf("[Send Packet] CreateFile = [%s] \n\n", path.c_str());
	// 메모리 초기화
	memset(&packet, 0x00, sizeof(PACKET_CreateFile));

	// 패킷이 어떤 패킷인지 ?
	packet.cmd = PROTOCOL_ID_CREATEFILE;

	// 패킷 정보 추가
	memcpy(&packet.pkt, path.c_str(), path.size());

	return TRUE;
}


BOOL GetCloseHandlePacket(string path, PACKET_CloseHandle& packet)
{
	printf("[Send Packet] CloseHandle \n\n", path.c_str());
	// 메모리 초기화
	memset(&packet, 0x00, sizeof(PACKET_CloseHandle));

	// 패킷이 어떤 패킷인지 ?
	packet.cmd = PROTOCOL_ID_CLOSEHANDLE;

	return TRUE;
}
