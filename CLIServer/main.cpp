#include <signal.h>
#include <iostream>
#include <thread>
#include <string>
#include <stdlib.h>

#include "TCPServer.h"
#include "HTTPServer.h"
#include "Util.h"

CHTTPServer http;
CTCPServer tcp;


void     INThandler(int);
static volatile int keepRunning = 1;

void PrintCommand();

// HTTP Callback
int cb_protocol(CTCPServer& tcp, SOCKET sock, string method, string uri, std::map<string, string> querystring, string& responseBody)
{
	if (method.compare("GET") == 0)
	{
		if (uri.compare("/SendAll") == 0)
		{
			if (querystring.find("filepath") != querystring.end() && querystring.find("filename") != querystring.end())
				tcp.SendAll(querystring["filepath"].c_str(), querystring["filename"].c_str());
			
			responseBody = "Hello, SendAll";
		}
		else if (uri.compare("/SendTo") == 0)
		{
			if (querystring.find("filepath") != querystring.end() && querystring.find("filename") != querystring.end() && querystring.find("address") != querystring.end())
				tcp.SendTo(querystring["address"].c_str(), querystring["filepath"].c_str(), querystring["filename"].c_str());

			responseBody = "Hello, SendTo";
		}
		else if (uri.compare("/HeartBeat") == 0)
		{
			tcp.HeartBeat(responseBody);
			//responseBody = "Heart Beat";
		}
		else if (uri.compare("/echo") == 0)
		{

		}

	}
	else if (method.compare("POST") == 0)
	{

	}

	return 0;
}

int main(int argc, const char* argv[])
{
	signal(SIGINT, INThandler);

	http.SetPort(5004);
	http.Bind();

	const long nPort = 5003;
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
		bool isHttp = http.Receive(tcp, cb_protocol);
		if (isHttp)
		{
			printf("Http Request ! \n");
		}
		else
		{
			//tcp.Receive(NULL); // 콜백 넣어서 처리 할 것

			// 클라이언트 리스트에 추가한다. 
			tcp.AddClient();

		}
	} // END #while_1

	// ===============================================================================
	http.Clear();
	tcp.Clear();
	// ===============================================================================
	return 0;
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

		//LoadINI(); 
		break;

	case 2:
	{
		Util::OpenFileDialog(szPath, MAX_PATH);
		// 경로에서 파일 이름만 가져온다.
		int nPosLastEscape = 0;
		for (nPosLastEscape = strlen(szPath) - 1; szPath[nPosLastEscape] != '\\'; --nPosLastEscape);
		const char* fileName = szPath + nPosLastEscape;
		
		tcp.SendAll(szPath, fileName);
		printf(">> %s \n", szPath);

		
	}
	break;
	case 3:
		//PrintClient(sockets);
		tcp.ShowClient();
		break;
	default:
		break;
	}

	system("pause");
}


void  INThandler(int sig)
{
	signal(sig, SIG_IGN);
	printf("Ctrl + C \n");
	keepRunning = 0;
}
