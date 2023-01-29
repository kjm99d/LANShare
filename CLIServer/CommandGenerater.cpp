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
	
	/*
	* length -> 실제 데이터의 길이
	* cmd -> 명령 정보를 할당받은 크기
	* 
	* EX)만약 데이터의 길이가 30바이트라면,
	*		명령어 4바이트 까지 해서
	*		총 34바이트를 전송한다.
	*/
	const int nLengthPacket = length + sizeof(cmd);

	ret = pool.Append((char*)(&nLengthPacket), sizeof(int));
	ret = pool.Append((char*)(&cmd), sizeof(cmd));

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
