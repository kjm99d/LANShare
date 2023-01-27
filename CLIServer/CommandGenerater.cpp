#include "CommandGenerater.h"

CCommandGenerater::CCommandGenerater(int cmd, int length)
{
	this->cmd = cmd;
	this->length = length;

	Run();
}

CCommandGenerater::~CCommandGenerater()
{
}

bool CCommandGenerater::Run()
{
	int ret = false;
	
	pool.SetPosition(0);
	ret = pool.Append((char*)(&cmd), sizeof(cmd));
	ret = pool.Append((char*)(&length), sizeof(length));

	return ret;
}


void CCommandGenerater::SetCommand(int cmd)
{
	this->cmd = cmd;
}

void CCommandGenerater::SetLength(int length)
{
	this->length = length;
}
