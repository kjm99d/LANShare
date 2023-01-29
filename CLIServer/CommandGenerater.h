/*****************************************************************//**
 * \file   CommandGenerater.h
 * \brief  단순히 명령어 정보만 받아서, 데이터를 저장하고, 해당 데이터의 버퍼시작 주소와 메모리크기(사이즈)를 구성해주는 클래스
 * \brief  생성자에 파라미터로 명령어 정보, 새롭게 채울 버퍼, 버퍼의 길이를 넣고 Run() 함수를 실행하여 메모리 풀을 구성한다.
 * \brief  메모리 풀 공간에 저장된 스트림 정보와 스트림의 위치를 기반으로 버퍼의 길이를 반환 받을 수 있다.
 *
 *  
 * \author KJM
 * \date   January 2023
 *********************************************************************/

#pragma once
#include "MemoryStreamPool.h"


class CCommandGenerater
{

public:
	CCommandGenerater(int cmd, int length);
	~CCommandGenerater();


public:
	const char * const GetBuffer() { return pool.GetBuffer(); }
	const int GetSize() { return pool.GetPosition() * sizeof(char); }

private:
	bool Run();
	void SetCommand(int cmd);
	void SetLength(int length);
	

private:
	int length;
	int cmd;
	
	CMemoryStreamPool<char> pool = CMemoryStreamPool<char>(8);
};

