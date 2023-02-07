
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


		const int nTotalPacket = *(int*)buffer;
		const int iCommand = *(int *)(buffer + 4);

		// CreateFile 이라면
		if (iCommand == PROTOCOL_ID_CREATEFILE)
		{
			// 실제 파일 길이는 이만큼
			const int nPath = nTotalPacket - sizeof(int) - sizeof(int);
			char* pFilePath = new char[nPath + 1];
			
			while (true)
			{
				int tmp_read_size = recv(client, (char*)pFilePath, nPath, 0);
				if (tmp_read_size != nPath) continue;
				pFilePath[nPath] = 0x00; // 마지막 위치를 NULL로 준다.
				CreateDirecotryStaticPath(pFilePath);
				printf(">> 파일 생성 :: %s \n", pFilePath);
				file.open(pFilePath, std::ios_base::binary);
				break;
			}

			delete[] pFilePath;

		}
		else if (iCommand == PROTOCOL_ID_WRITEFILE)
		{
			// 실제 파일 길이는 이만큼
			const int ExistsPackets = nTotalPacket - sizeof(int) - sizeof(int);
			recv(client, (char*)buffer, ExistsPackets, 0);
			const int iFileSize = *(int *)buffer;

			size_t readStack = 0;

			while (true)
			{
				char buffer[4096] = { 0, };
				const int buffer_size = sizeof(buffer) / sizeof(char);
				int tmp_read_size = 0;
				
				if (readStack + buffer_size <= iFileSize)
					tmp_read_size = recv(client, (char *)buffer, buffer_size, 0);
				else
					tmp_read_size = recv(client, (char*)buffer, iFileSize - readStack, 0);

				
				if (tmp_read_size == 0 || tmp_read_size == -1) continue;
				else readStack += tmp_read_size;

				//send(client, "1", 1, 0);
				static int c = 1;
				
				if (file.write(buffer, tmp_read_size))
				{
					size_t numberOfBytesWritten = file.tellp();
					printf(">> NoB [%04d] [%04d] [%f%] Current Length = [%10d]\n", numberOfBytesWritten, c++, readStack / (float)iFileSize * 100.0f, tmp_read_size);

				}
				//fwrite(buffer, 1, tmp_read_size, file.);

				if (iFileSize == readStack)
					break;
				else if (readStack > iFileSize)
					break;
			}
		}
		else if (iCommand == PROTOCOL_ID_CLOSEHANDLE)
		{
			printf(">> 파일 다운로드 완료");
			file.close();
		}
		else if (iCommand == PROTOCOL_ID_HEARTBEAT)
		{
			printf(">> LIVE !");
			char result = 0x01;
			send(client, &result, 1, 0);
		}
		else if (iCommand == PROTOCOL_ID_ECHO)
		{
			char* fileName = (char*)malloc(nTotalPacket - 4 + 1);
			const size_t sMesageSize = nTotalPacket - 4;
			
			while (true) {
				int tmp_read_size = recv(client, (char*)fileName, nTotalPacket - 4, 0);
				if (tmp_read_size != (nTotalPacket - 4)) continue;
				fileName[nTotalPacket - 4 + 0] = 0x00;
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
