

#include "TCPServer.h"
#include "HTTPServer.h"

#include <vector>
#include <signal.h>
#include <iostream>
#include <thread>


#include "INISettingLoader.h"
#include "ProtocolBase.h"
#include "ProtocolHTTP.h"

#include "ProtocolID.h"			// 패킷에 정의될 프로토콜 ID
#include "CLIPacketStruct.h"	// 패킷정의 할 구조체 정보
#include "MemoryPool.h"
#include "MemoryStreamPool.h"
#include "CommandGenerater.h"
#include "BufferWriter.h"
#include "FileReader.h"

#include <string>
#include <stdlib.h>




void     INThandler(int);
static volatile int keepRunning = 1;


BOOL OpenFileDialog(char * buffer, int length)
{
	OPENFILENAMEA ofn;       // common dialog box structure

	// Initialize OPENFILENAME
	//ZeroMemory(&ofn, sizeof(ofn));
	memset(&ofn, 0x00, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = buffer;
	ofn.nMaxFile = length;
	ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameA(&ofn) == TRUE)
	{
		// use ofn.lpstrFile
		//if (length > 0)
			//memcpy_s(buffer, length, ofn.lpstrFile, sizeof(ofn.lpstrFile) / sizeof(char));
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

/**
 * 파일을 클라이언트에게 전달한다..
 * 
 * \param src - 서버PC 내 보낼파일의 경로
 * \param file_name - 클라이언트가 다운 받았을 때, 저장될 파일명 (확장자 포함)
 * \return 
 */
BOOL SendFile(const char* src, const char* file_name);

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
	signal(SIGINT, INThandler);

	CHTTPServer http;
	http.SetPort(5004);
	http.Bind();

	const long nPort = 5003;
	CTCPServer tcp;
	tcp.SetPort(nPort);
	tcp.Bind();

	// 명령어 입력 받기
	std::thread CommandListener = std::thread([] {	
		while (true) {
			system("cls"); // 콘솔창 지워라 
			PrintCommand();
		}
		});

	while (keepRunning)
	{
#if 0
		// 패킷을 수신한다
		SOCKET Sock = tcp.GetListen();
		
		RecvPacket(sockets);
		AddClient(sockets, Sock);
#else
		http.Receive(NULL);
#endif


	} // END #while_1

	// ===============================================================================
	http.Clear();
	tcp.Clear();
	// ===============================================================================
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

		CProtocolBase* protocol = new CProtocolHTTP();
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

BOOL AddClient(std::vector<std::pair<SOCKET, SOCKADDR_IN>>& sockets, SOCKET& sock_tcp_listen)
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
	char szPath[MAX_PATH] = { 0, };
	switch (menu_num)
	{
	case 1:

		Cur.X = 0;
		Cur.Y += 2;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);

		LoadINI(); 
		break;

	case 2:
	{
		OpenFileDialog(szPath, MAX_PATH);
		int nPosLastEscape = 0;
		for (nPosLastEscape = strlen(szPath) - 1; szPath[nPosLastEscape] != '\\'; --nPosLastEscape);
		SendFile(szPath, &szPath[nPosLastEscape]);
		printf(">> %s \n", szPath);
	}
		break;
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
	for (int i = 0; i < sockets.size(); ++i)
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


	
	return 0;
}


void  INThandler(int sig)
{
	signal(sig, SIG_IGN);
	printf("Ctrl + C \n");
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

BOOL SendFile(const char* src, const char* file_name)
{
	std::string dst = "C:\\LanShare\\";
	dst += file_name;

	CBufferWriter buffer_writer;

	// 파일 생성
	CCommandGenerater create_file(PROTOCOL_ID_CREATEFILE, (int)dst.size());
	for (int i = 0; i < sockets.size(); ++i)
	{
		buffer_writer.Write(sockets[i].first, create_file); // 헤더 전송
		buffer_writer.Write(sockets[i].first, (char*)dst.c_str(), dst.size()); // 데이터 전송
	}


	// 파일 버퍼 쓰기
	CFileReader* reader = new CFileReader(4096, (char*)src);
	size_t file_size = reader->FileSize();

	CCommandGenerater WriteFileHeader(PROTOCOL_ID_WRITEFILE, (int)file_size);
	for (int i = 0; i < sockets.size(); ++i)
		buffer_writer.Write(sockets[i].first, WriteFileHeader); // 헤더 전송

	while (true)
	{
		// 서버 PC 에서 파일을 읽고
		const char* const file_buf = reader->GetBuffer();
		const size_t buffer_size = reader->GetBufferSize();

		// 파일이 끝이면 그만 보내라
		if (buffer_size == 0)
			break;

		// 헤더에 명령줄이랑 현재 시점에 읽은 버퍼 크기 넣고 
		//CCommandGenerater write_file(PROTOCOL_ID_WRITEFILE, buffer_size);
		for (int i = 0; i < sockets.size(); ++i)
		{
			// 쏜다 !
			//buffer_writer.Write(sockets[i].first, write_file); // 헤더 전송
			buffer_writer.Write(sockets[i].first, (char*)file_buf, buffer_size); // 데이터 전송
		}
	}

	delete reader;

	// 파일 핸들 닫기
	CCommandGenerater close_handle(PROTOCOL_ID_CLOSEHANDLE, 0);
	for (int i = 0; i < sockets.size(); ++i)
	{
		const char* const b = close_handle.GetBuffer();
		const int sz = close_handle.GetSize();
		send(sockets[i].first, b, sz, 0);
	}


	return TRUE;
}