#include "Server.h"

// ================================================== //
IServer::IServer() : port(0), listen_sock(0)
{
}

IServer::~IServer()
{
}

void IServer::SetPort(long port)
{
	this->port = port;
}

BOOL IServer::Bind()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		//printf("WSAStartup Failure \n");
		return FALSE;
	}

	// 리턴값을 저장시킬 변수
	int ret_value = 0;

	// socket() - tcp 소켓 생성
	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) {
		//printf("Create TCP Socket Failure \n");
		return FALSE;
	}

	// 논블로킹 소켓으로 전환
	u_long sock_on = 1;
	ret_value = ioctlsocket(listen_sock, FIONBIO, &sock_on);
	if (ret_value == SOCKET_ERROR) {
		//printf("ioctlsocket() Failure \n");
		return FALSE;
	}

	// SO_REUSEADDR 소켓 옵션 설정(REUSEADDR)
	BOOL optReuse = TRUE;
	ret_value = setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&optReuse, sizeof(optReuse));
	if (ret_value == SOCKET_ERROR) {
		//printf("setsockopt() REUSEADDR Failure \n");
	}

	// bind() - 서버의 지역 IP주소와 지역포트 번호를 위한 구조체 전달
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr.S_un.S_addr = htons(INADDR_ANY);
	ret_value = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (ret_value == SOCKET_ERROR) {
		//printf("bind() Failure \n");
		return FALSE;
	}


	// listen() - bind에 의해 처리된 tcp 포트의 상태정보를 LISTEN으로 바꾼다.
	ret_value = listen(listen_sock, SOMAXCONN);
	if (ret_value == SOCKET_ERROR) {
		//printf("listen() Failure \n");
		return FALSE;
	}

	return TRUE;
}

void IServer::Clear()
{
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
}

bool IServer::Accept(SOCKET& ref_socket)
{
	int addrlen = sizeof(SOCKADDR_IN);
	
	SOCKADDR_IN clientaddr;
	
	return Accept(ref_socket, clientaddr);
}

bool IServer::Accept(SOCKET& ref_socket, SOCKADDR_IN& ref_addr_in)
{
	int addrlen = sizeof(SOCKADDR_IN);

	// accept() - 서버에 접속한 클라이언트와 통신할 수 있도록 새로운 소켓을 생성리턴한다.
	ref_socket = accept(listen_sock, (SOCKADDR*)&ref_addr_in, &addrlen);

	if (ref_socket == INVALID_SOCKET)
		return false;
	return true;
}

void IServer::SafeSend(SOCKET& ref_sock, char* buffer, int length)
{
	int countdown = length;

	while (countdown > 0)
	{
 		int result = send(ref_sock, (char*)buffer + (length - countdown), countdown, 0);
		const int err = WSAGetLastError();


		if (err == WSAECONNRESET && result == SOCKET_ERROR)
			break;
		if (result == SOCKET_ERROR)
			continue;

		countdown -= result;
	}
}

void IServer::SafeSend(SOCKET& ref_sock, const std::string & buffer)
{
	// Safe Code : SafeSend(ref_sock, (char *)buffer.c_str(), buffer.size());

	int countdown = buffer.length();

	while (countdown > 0)
	{
		int result = send(ref_sock, (char*)buffer.c_str() + (buffer.length() - countdown), countdown, 0);
		const int err = WSAGetLastError();


		if (err == WSAECONNRESET && result == SOCKET_ERROR)
			break;
		if (result == SOCKET_ERROR)
			continue;

		countdown -= result;
	}

}

bool IServer::SafeRecv(SOCKET& ref_sock, std::string& ref_buffer)
{
	unsigned char buffer[4096] = { 0, };
	const size_t buffer_size = sizeof(buffer) / sizeof(unsigned char);

	int iCountFailure = 0;

	while (true)
	{
		if (iCountFailure == 100)
			return false;

		const int recv_size = recv(ref_sock, (char*)buffer, buffer_size, 0);
		const int err = WSAGetLastError();

		//printf("Receive Size = [%d] LastError = [%d] \n", recv_size, err);
		if (err == WSAECONNRESET && recv_size == SOCKET_ERROR)
			break;
		if (WSAEWOULDBLOCK == err)
		{
			Sleep(10);
			++iCountFailure;
			continue;
		}

		ref_buffer.append((char*)buffer, recv_size);
		if (recv_size < buffer_size && 0 == err)
			return true;
	}

	
}

// ================================================== //







