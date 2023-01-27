
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#pragma comment (lib , "ws2_32.lib")

#include "ProtocolID.h"			// 패킷에 정의될 프로토콜 ID
#include "CLIPacketStruct.h"	// 패킷정의 할 구조체 정보

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

    while (true) {
        unsigned char buffer[512] = { 0, };
        const int length = recv(client, (char*)buffer, 260, 0);
        if (length == 0) {
            break;
        }
        else if (length == -1)
        {
            continue;
        }

        if (length > 0) 
        {
            const int * cmd = (int *)buffer;
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
                    char * m = (char *)malloc(kFs +1);
                    memset(m, 0x00, kFs +1);
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
