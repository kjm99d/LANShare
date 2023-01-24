#pragma once
#include <stdlib.h>
#include <minwindef.h>

typedef struct PACKET_Command
{
	int cmd;
};

typedef struct PACKET_CreateFile : public PACKET_Command
{
	char pkt[MAX_PATH];
};

typedef struct PACKET_CloseHandle : public PACKET_Command
{

};

typedef struct PACKET_WriteFile: public PACKET_Command
{
	char buffer[512];
};

