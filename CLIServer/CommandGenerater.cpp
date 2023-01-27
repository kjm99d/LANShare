#include "CommandGenerater.h"

CCommandGenerater::CCommandGenerater(int cmd, char* buffer, int length)
{
	this->cmd = cmd;
	this->buffer = buffer;
	this->length = length;
}

CCommandGenerater::~CCommandGenerater()
{
}

bool CCommandGenerater::Run()
{
	int ret = false;
	
	pool.SetPosition(0);
	ret = pool.Append((char*)(&cmd), sizeof(cmd));
	ret = pool.Append((char*)buffer, length);

	return ret;
}

void CCommandGenerater::SetBufferPosition(int pos)
{
	pool.SetPosition(sizeof(this->cmd) + pos);
}

void CCommandGenerater::SetCommand(int cmd)
{
	this->cmd = cmd;
}

void CCommandGenerater::SetBuffer(char* buffer, int length)
{
	this->buffer = buffer;
	this->length = length;
}
