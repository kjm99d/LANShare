/*****************************************************************//**
 * \file   BufferWriter.h
 * \brief 
 * 
 * \author KJM
 * \date   January 2023
 *********************************************************************/
#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>

#include "CommandGenerater.h"

class CBufferWriter
{
public:
	bool Write(SOCKET& socket, CCommandGenerater& cmd);
	bool Write(SOCKET& socket, void* buffer, int buffer_size);
	


};

