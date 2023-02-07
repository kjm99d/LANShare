
#include "TCPClient.h"

#include "ProtocolID.h"			// 패킷에 정의될 프로토콜 ID

#include "INISettingLoader.h"

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")


#include <fstream>


bool CreateDirecotryStaticPath(std::string path);

int main(int argc, const char* argv[])
{
	// 연결할 소켓 서버 주소와 포트 가져오기
	CINISettingLoader settings(".\\settings.ini");
	settings.SetSection("Client");
	std::string host;
	UINT port;
	settings.Get("host", host);
	settings.Get("port", port);
	if (host.compare("NONE") == 0) host = "127.0.0.1";
	if (0 == port) port = 5003;

	// 연결하기
	CTCPClient connecter(host, port);
	connecter.Connect();

	/*
	FILE* fd = nullptr;
	*/
	std::ofstream file;


	while (true) {
		SOCKET client = connecter.GetSock();
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


		const int pkt_size = *(int*)buffer;
		const int Cmd = *(int *)(buffer + 4);

		// CreateFile 이라면
		if (Cmd == PROTOCOL_ID_CREATEFILE)
		{
			char* fileName = (char *)malloc(pkt_size - 4 + 1);
			
			while (true)
			{
				int tmp_read_size = recv(client, (char*)fileName, pkt_size - 4, 0);
				if (tmp_read_size != (pkt_size - 4)) continue;
				fileName[pkt_size - 4 + 0] = 0x00;
				CreateDirecotryStaticPath(fileName);
				printf(">> 파일 생성 :: %s \n", fileName);
				file.open(fileName, std::ios_base::binary);
				//fopen_s(&fd, fileName, "wb");
				break;
			}

			delete fileName;

		}
		else if (Cmd == PROTOCOL_ID_WRITEFILE)
		{
			
#if 0
			char* file_data = (char*)malloc(pkt_size - 4);

			while (true)
			{
				int tmp_read_size = recv(client, (char*)file_data, pkt_size - 4, 0);
				if (tmp_read_size != (pkt_size - 4)) continue;


				fwrite(file_data, 1, tmp_read_size, fd);
				break;
			}

			delete file_data;
#else
			const size_t cFileSize = pkt_size - 4;
			size_t readStack = 0;

			while (true)
			{
				char buffer[4096] = { 0, };
				const int buffer_size = sizeof(buffer) / sizeof(char);
				int tmp_read_size = 0;
				
				if (readStack + buffer_size <= cFileSize)
					tmp_read_size = recv(client, (char *)buffer, buffer_size, 0);
				else
					tmp_read_size = recv(client, (char*)buffer, cFileSize - readStack, 0);

				
				if (tmp_read_size == 0 || tmp_read_size == -1) continue;
				else readStack += tmp_read_size;

				//send(client, "1", 1, 0);
				static int c = 1;
				
				if (file.write(buffer, tmp_read_size))
				{
					size_t numberOfBytesWritten = file.tellp();
					printf(">> NoB [%04d] [%04d] [%f%] Current Length = [%10d]\n", numberOfBytesWritten, c++, readStack / (float)cFileSize * 100.0f, tmp_read_size);

				}
				//fwrite(buffer, 1, tmp_read_size, file.);

				if (cFileSize == readStack)
					break;
				else if (readStack > cFileSize)
					break;
			}

#endif
		}
		else if (Cmd == PROTOCOL_ID_CLOSEHANDLE)
		{
			printf(">> 파일 다운로드 완료");
			file.close();
		}
		else if (Cmd == PROTOCOL_ID_HEARTBEAT)
		{
			printf(">> LIVE !");
			char result = 0x01;
			send(client, &result, 1, 0);
		}
		else if (Cmd == PROTOCOL_ID_ECHO)
		{
			char* fileName = (char*)malloc(pkt_size - 4 + 1);
			const size_t sMesageSize = pkt_size - 4;
			
			while (true) {
				int tmp_read_size = recv(client, (char*)fileName, pkt_size - 4, 0);
				if (tmp_read_size != (pkt_size - 4)) continue;
				fileName[pkt_size - 4 + 0] = 0x00;
				break;
			}

			MessageBoxA(NULL, fileName, "M", MB_OK);

			delete fileName;
		}

	}

	connecter.Close();

	return 0;
}

bool CreateDirecotryStaticPath(std::string path)
{
	const std::string strFindString = "\\";
	const size_t nFindStringSize = strFindString.length();
	

	size_t pos = 0;
	while (true)
	{

		pos = path.find(strFindString, pos);
		if (pos == std::string::npos)
			break;

		// ================================== //
		std::string strTemp = path.substr(0, pos);
		if (PathFileExistsA(strTemp.c_str()) != 1) 
			CreateDirectoryA(strTemp.c_str(), 0);
		pos += nFindStringSize;

	}

	return true;
}
