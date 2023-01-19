
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#pragma comment (lib , "ws2_32.lib")

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

    while (true) {
        unsigned char buffer[1024] = { 0, };
        const int length = recv(client, (char*)buffer, 1024, 0);
        if (length == 0) {
            break;
        }
        else if (length == -1)
        {
            continue;
        }

        if (length > 0) 
        {
            printf("%d \n", length);
            count += length;
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
