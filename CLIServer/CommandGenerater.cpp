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

	ret = pool.Append((char*)(&cmd), sizeof(cmd));
	ret = pool.Append((char*)buffer, length);

	return ret;
}
