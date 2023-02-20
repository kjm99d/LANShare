#include "Socket.h"

CSocket::CSocket(int port, std::string address) : m_port(port), m_address(address), m_socket(0)
{
}

CSocket::~CSocket()
{
}

bool CSocket::SafeSend(const std::string& ref_buffer)
{
}

bool CSocket::SafeRecv(const std::string& ref_buffer)
{
	return false;
}


int CSocket::Connect()
{
	return 0;
}

int CSocket::Disconnect()
{
	return 0;
}
