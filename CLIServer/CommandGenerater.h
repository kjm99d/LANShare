#pragma once
#include "MemoryStreamPool.h"


class CCommandGenerater
{

public:
	CCommandGenerater(int cmd, char * buffer, int length);
	~CCommandGenerater();

public:
	bool Run();
	const char * const GetBuffer() { return pool.GetBuffer(); }
	const int GetSize() { return pool.GetPosition() * sizeof(char); }

private:
	int cmd;
	char* buffer;
	int length;
	
	CMemoryStreamPool<char> pool = CMemoryStreamPool<char>(512);
};

