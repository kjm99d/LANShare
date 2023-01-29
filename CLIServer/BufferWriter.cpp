#include "BufferWriter.h"

bool CBufferWriter::Write(SOCKET& socket, void* buffer, int buffer_size)
{
	const int result = send(socket, (char *)buffer, buffer_size, 0);
	return result;
}

bool CBufferWriter::Write(SOCKET& socket, CCommandGenerater& cmd)
{
	const int result = send(socket, cmd.GetBuffer(), cmd.GetSize(), 0);
	return result;
}
 