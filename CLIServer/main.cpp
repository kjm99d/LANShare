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

CHTTPServer http;
CTCPServer tcp;


void     INThandler(int);
static volatile int keepRunning = 1;

void PrintCommand();

IHTTPResponse* GetControllerV2(CTCPServer& tcp, std::string uri, std::map<string, string> querystring, ResponseDispatcher& dispatcher)
{
	if (uri.compare("/SendTo") == 0)
	{
		Json::Value json;
		json["a"] = "hello";

		return dispatcher.JSON(200, json);
	}
	else if (uri.compare("/ftp") == 0)
	{
		auto elem = querystring.find("path");
		if (elem == querystring.end())
		{

		} 
		else
		{
			auto path = elem->second;
			path = Util::URLDecode(path);
			std::cout << "Input Path = " << path << std::endl;

			return dispatcher.JSON(200, tcp.GetFileList(path), "*");
		}

	}
	else if (uri.compare("/ResponseFromText") == 0)
	{

		std::string text = "";
		text += "<!DOCTYPE html>\n";
		text += "<head><title>hello</title></head>\n";
		text += "<body><h1>world</h1></body>\n";


		return dispatcher.Text(200, text);
	}
	else if (uri.compare("/HeartBeat") == 0)
	{
		return dispatcher.JSON(200, tcp.HeartBeat(), "*");
	}
	else
	{
		// Favicon.ico 같은 것들을 여기서 처리를 해주던가
		// 404 처리를 하던가 해야함
		std::string text = "";

		return dispatcher.Text(404, text);

	}
}

// HTTP Callback
std::string GetController(CTCPServer& tcp,std::string uri, std::map<string, string> querystring)
{

	return 0;
}

std::string PostController(CTCPServer& tcp, std::string uri, std::map<string, string> querystring, map<string, string> queryPayloads, Json::Value jsonPayloads)
{
	if (uri.compare("/SendAll") == 0)
	{
		Json::Value filepath = jsonPayloads["filepath"];
		Json::Value filename = jsonPayloads["filename"];
		if (filepath.isNull() == false && filename.isNull() == false)
			tcp.SendAll(filepath.asString().c_str(), filename.asString().c_str());
		
		// Feature : 
		// JSON 객체로 만들어서 뱉어주고
		// 밖에서는 인터페이스로 받아오고

		return "Hello, SendAll";
	}
	else if (uri.compare("/SendTo") == 0)
	{
		if (queryPayloads.find("filepath") != queryPayloads.end() && queryPayloads.find("filename") != queryPayloads.end() && queryPayloads.find("address") != queryPayloads.end())
			tcp.SendTo(queryPayloads["address"].c_str(), queryPayloads["filepath"].c_str(), queryPayloads["filename"].c_str());

		return "Hello, SendTo";
	}
	else if (uri.compare("/HeartBeat") == 0)
	{
		//tcp.HeartBeat(responseBody);
		//responseBody = "Heart Beat";
	}
	else if (uri.compare("/echo") == 0)
	{

	}

	return 0;
}

int main(int argc, const char* argv[])
{
	
	CFileLogger* logger = new CFileLogger("log.txt");
	logger->LogWrite(__FUNCTION__, __LINE__, "Program Starting ...");

	signal(SIGINT, INThandler);

	http.SetPort(5004);
	http.Bind();

	http.SetController(GetController);
	http.SetController(PostController);
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
