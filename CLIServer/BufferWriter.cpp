#include "BufferWriter.h"

/**
 * 소켓 정보를 전달받아, 해당 소켓에 데이터를 전송한다..
 * 
 * \param socket - 소켓 레퍼런스
 * \param buffer - 전달될 데이터
 * \param buffer_size - 전달될 데이터의 크기
 * \return 
 */
bool CBufferWriter::Write(SOCKET& socket, void* buffer, int buffer_size)
{
	int countdown = buffer_size;
	
	while (countdown > 0) 
	{
		int result = send(socket, (char*)buffer + (buffer_size - countdown), countdown, 0);
		
		if (result == -1)
			continue;

		countdown -= result;
	}
	
	return true;
}

/**
 * 소켓 정보를 전달받아, 커맨드를 전송한다.
 * 
 * \param socket - 소켓 정보
 * \param cmd - 커맨드제너레이터 객체 레퍼런스
 * \return 
 */
bool CBufferWriter::Write(SOCKET& socket, CCommandGenerater& cmd)
{
	int countdown = cmd.GetSize();

	while (countdown > 0)
	{
		int result = send(socket, (char*)cmd.GetBuffer() + (cmd.GetSize() - countdown), countdown, 0);
		const int err = WSAGetLastError();

		if (result == -1)
			continue;

		countdown -= result;
	}

	return true;
}
 