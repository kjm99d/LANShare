#include <signal.h>
#include <iostream>
#include <thread>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <istream>

#include "Logger.h"
#include "ConsoleLogger.h"
#include "FileLogger.h"

#include "TCPServer.h"
#include "HTTPServer.h"
#include "Util.h"
#include "CreateMutex.h"

CHTTPServer http;
CTCPServer tcp;


void     INThandler(int);
static volatile int keepRunning = 1;

void PrintCommand();

IHTTPResponse* GetControllerV2(CTCPServer& tcp, std::string uri, std::map<string, string> querystring, ResponseDispatcher& dispatcher)
{
	int iStatusCode = 200;
	std::string strMessage = "";

	std::cout << uri;

	if (uri.compare("/status") == 0)
	{
		return dispatcher.JSON(200, "success", NULL, "*");
	}
	else if (uri.compare("/ftp") == 0)
	{
		std::cout << "?path=" << querystring["path"] << std::endl;

		Json::Value objs;

		iStatusCode = 401;
		strMessage = "QueryString Path is not found";

		auto elem = querystring.find("path");
		if (elem != querystring.end())
		{
			auto path = elem->second;
			path = Util::URLDecode(path);
			objs = tcp.GetFileList(path);
			iStatusCode = 200;
			strMessage = "success";
		}

		return dispatcher.JSON(iStatusCode, strMessage, objs, "*");
	}
	else
	{
		// 404 페이지 전달 하는 곳
		return dispatcher.Text(404, "");

	}
}

int main(int argc, const char* argv[])
{
	CCreateMutex MyMutex("LANShare.Server.Lock");
	if (MyMutex.exist())
	{
		MessageBoxA(NULL, "동시 실행", "", MB_OK);
		return -1;
	}

	CFileLogger* logger = new CFileLogger("log.txt");
	logger->LogWrite(__FUNCTION__, __LINE__, "Program Starting ...");

	signal(SIGINT, INThandler);

	http.SetPort(5004);
	http.Bind();

	http.SetController(GetControllerV2);

	constexpr long nPort = 5003;
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
		// CoWork 될 타입을 매개변수로 주고 Callback을 호출한다.
		bool isHttp = http.ReceiveV2(tcp);
		if (isHttp)
		{
			
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

	delete logger;
	
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
	printf("| [1] 다이얼로그로 읽기                                      | \n");
	printf("| [2] 현재 연결된 모든 클라이언트 정보 출력                  | \n");
	printf("| [3] 클라이언트 상태 확인하기                               | \n");
	printf("| [4] 에코 메세지 전송하기                                   | \n");
	printf("| [5] CommandLine 명령 전송하기                              | \n");
	printf("| [6] 파일목록 조회하기                                      | \n");
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

	//scanf_s(" %d", &menu_num);
	cin >> menu_num;
	Cur.X = 0;
	Cur.Y += 2;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);

	// Selector >> 나중에 함수로 별도로 만들어 둘 것
	// 입출력 부분과 이벤트 트리거 해주는 부분은 달라야함.
	char szPath[MAX_PATH] = { 0, };
	switch (menu_num)
	{
	case 1:
	{
		if (Util::OpenFileDialog(szPath, MAX_PATH) == TRUE)
		{
			// 경로에서 파일 이름만 가져온다.
			int nPosLastEscape = 0;
			for (nPosLastEscape = strlen(szPath) - 1; szPath[nPosLastEscape] != '\\'; --nPosLastEscape);
			const char* fileName = szPath + nPosLastEscape;

			tcp.SendAll(szPath, fileName);
			printf(">> %s \n", szPath);
		}
	}
	break;
	case 2:
		tcp.ShowClient();
		break;

	case 3:
	{
		std::string body;
		tcp.HeartBeat(body);
		std::cout << body << std::endl;
	}
		break;

	case 4:
	{
		std::string msg;
		cout << "Message >> ";
		cin.get();
		getline(cin, msg);
		//cin.getline(msg, 1024);

		tcp.Echo(msg);
		break;
	}
	case 5:
	{
		std::string cmd;
		cout << "CommandLine (exit: q) >> ";
		cin.get();
		getline(cin, cmd);
		//cin.getline(msg, 1024);
		if (cmd.compare("q") != 0)
			tcp.CommandLine(cmd);
		break;
	}
	case 6:
	{
		std::string dir;
		std::getline(cin, dir);
		std::getline(cin, dir);
		const auto & files = tcp.GetFileList(dir);
		for (auto file : files)
		{
			std::cout << "Directory : " << file["is_dir"] << ", ";
			std::cout << "Name : " << file["name"] << " " << std::endl;;
		}
	}
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
