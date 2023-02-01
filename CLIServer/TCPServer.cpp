#include "TCPServer.h"

CTCPServer::CTCPServer()
{
}

CTCPServer::~CTCPServer()
{
}

int CTCPServer::Receive(fp_TCPEvent cb_callback)
{


	// 콜백 함수 
	if (cb_callback)
	{
		cb_callback(0);
	}

	return 0;
}
