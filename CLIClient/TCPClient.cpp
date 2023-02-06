#include "TCPClient.h"

// ================================================================================ //
// 생성자/소멸자
// ================================================================================ //
CTCPClient::CTCPClient(string address, int port) : m_socket(0), m_address(address), m_port(port)
{
}

CTCPClient::~CTCPClient()
{
}

SOCKET CTCPClient::GetSock()
{
	return m_socket;
}

// ================================================================================ //
int CTCPClient::Connect()
{
	WSADATA data;
	::WSAStartup(MAKEWORD(2, 2), &data);

	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (m_socket == INVALID_SOCKET)
		return ERR_TCP_INVALID_SOCKET;

	sockaddr_in addr = { 0 };

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(m_address.c_str());
	addr.sin_port = htons(m_port);

	if (connect(m_socket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
		return ERR_TCP_CONNECT_ERROR;

	// 논블로킹 소켓으로 변경한다.
	SetNonBlocking();

	return 1;
}

int CTCPClient::Close()
{
	closesocket(m_socket);
	::WSACleanup();

	return 1;
}

void CTCPClient::SetNonBlocking()
{
	u_long nonBlockingMode = 1;
	ioctlsocket(m_socket, FIONBIO, &nonBlockingMode); // sock을 논블로킹 모드로 설정
}

void CTCPClient::SafeSend(char* buffer, int length)
{
	int countdown = length;

	while (countdown > 0)
	{
		int result = send(m_socket, (char*)buffer + (length - countdown), countdown, 0);
		const int err = WSAGetLastError();


		if (err == WSAECONNRESET && result == SOCKET_ERROR)
			break;
		if (result == SOCKET_ERROR)
			continue;

		countdown -= result;
	}
}

bool CTCPClient::SafeRecv(std::string& ref_buffer)
{
	unsigned char buffer[4096] = { 0, };
	const int buffer_size = sizeof(buffer) / sizeof(char);
	int iCountFailure = 0;
	while (true)
	{
		if (iCountFailure == 100)
			return false;

		const int recv_size = recv(m_socket, (char*)buffer, buffer_size, 0);
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
// ================================================================================ //

