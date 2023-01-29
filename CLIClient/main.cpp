
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#pragma comment (lib , "ws2_32.lib")

#include "ProtocolID.h"			// 패킷에 정의될 프로토콜 ID
#include "CLIPacketStruct.h"	// 패킷정의 할 구조체 정보

#include <queue>
#include <deque>

void showError(const char* msg);

int main(int argc, const char* argv[])
{
	WSADATA data;
	::WSAStartup(MAKEWORD(2, 2), &data);

	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (client == INVALID_SOCKET)
		showError("클라이언트 생성 실패");

	sockaddr_in addr = { 0 };

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(5003);

	if (connect(client, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
		showError("연결 실패");

	u_long nonBlockingMode = 1;

	ioctlsocket(client, FIONBIO, &nonBlockingMode); // sock을 논블로킹 모드로 설정


	unsigned long long count = 0;

	static FILE* fd = nullptr;
	static std::queue<char> q;



	while (true) {
		unsigned char buffer[512] = { 0, };
		const int read_size = recv(client, (char*)buffer, 8, 0);


		if (read_size == 0) {
			break;
		}
		else if (read_size == -1)
		{
			continue;
		}

		if (read_size < 0) continue;


		// 일단 보내자 
#if 0
		if (length > 0)
		{
			continue;
			const int* cmd = (int*)buffer;
			switch (*cmd)
			{
			case PROTOCOL_ID_CREATEFILE:
			{

				int FileSize = *(int*)(buffer + 4);
				int kFs = FileSize;
				//memset(buffer, 0x00, 512);
				printf("CreateFile Length = [%d] [%d] ", FileSize, kFs);

				while (true)
				{
					const int ReadSize = recv(client, (char*)buffer, 260, 0);
					if (FileSize == 0)
						break;

					if (FileSize - ReadSize >= 0)
					{
						FileSize -= ReadSize;
					}
					else
					{
						FileSize -= FileSize;
					}

				}
				char* m = (char*)malloc(kFs + 1);
				memset(m, 0x00, kFs + 1);
				memcpy(m, buffer, kFs);
				printf("[%s] \n", m);

				fopen_s(&fd, (char*)m, "wb");
				delete m;
			}
			break;
			case PROTOCOL_ID_WRITEFILE:
			{
				int FileSize = *(int*)(buffer + 4);
				printf("CreateFile Length = [%d] \n", FileSize);
				memset(buffer, 0x00, 512);

				while (true)
				{
					const int ReadSize = recv(client, (char*)buffer, 260, 0);
					if (FileSize == 0)
						break;

					if (FileSize - ReadSize >= 0)
					{
						fwrite(buffer, 1, length, fd);
						FileSize -= ReadSize;
					}
					else
					{
						fwrite(buffer, 1, FileSize, fd);
						FileSize -= FileSize;
					}

				}
			}
			break;
			case PROTOCOL_ID_CLOSEHANDLE:
				printf("CloseHandle \n");
				fclose(fd);
				fd = nullptr;
				break;
			}
		}
#endif


		const int pkt_size = *(int*)buffer;
		char * pool = (char *)malloc(pkt_size);
		memcpy_s(pool, pkt_size, buffer, pkt_size);

		const int Cmd = *(int *)(pool + 4);
		// CreateFile 이라면
		if (Cmd == PROTOCOL_ID_CREATEFILE)
		{
			char* fileName = (char *)malloc(pkt_size - 4);
			while (true)
			{
				int tmp_read_size = recv(client, (char*)fileName, pkt_size - 4, 0);
				if (tmp_read_size != (pkt_size - 4)) continue;

				printf("CreateFile = %s \n", fileName);
				fopen_s(&fd, fileName, "wb");
				break;
			}

			delete fileName;

		}
		else if (Cmd == PROTOCOL_ID_WRITEFILE)
		{
			char* file_data = (char*)malloc(pkt_size - 4);

			while (true)
			{
				int tmp_read_size = recv(client, (char*)file_data, pkt_size - 4, 0);
				if (tmp_read_size != (pkt_size - 4)) continue;


				fwrite(file_data, 1, tmp_read_size, fd);
				break;
			}

			delete file_data;
		}
		else
		{
			printf("파일 다 씀 !");
			
			//char* file_data = (char*)malloc(pkt_size - 4);
			//int tmp_read_size = recv(client, (char*)file_data, pkt_size - 4, 0);

			fclose(fd); 
			fd = nullptr;

			//delete file_data;
		}

		delete pool;
	}

	printf("%d \n", count);



	closesocket(client);
	::WSACleanup();

	return 0;
}


void showError(const char* msg)
{
	std::cout << "에러 : " << msg << std::endl;
	exit(-1);
}
