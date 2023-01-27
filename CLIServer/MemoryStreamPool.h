/*****************************************************************//**
 * \file   MemoryStreamPool.h
 * \brief  메모리풀을 스트림 형태로 사용할 수 있는 클래스
 * 
 * \author KJM
 * \date   January 2023
 *********************************************************************/
#pragma once
#include "MemoryPool.h"



template <typename T>
class CMemoryStreamPool : public CMemoryPool<T>
{
public:
	CMemoryStreamPool(int length);
	~CMemoryStreamPool();

public:
	bool Append(T* data, int length);
	int GetPosition();

private:
	int pos;
};

template<typename T>
CMemoryStreamPool<T>::CMemoryStreamPool(int length) : CMemoryPool<T>(length), pos(0)
{
	
}


template<typename T>
CMemoryStreamPool<T>::~CMemoryStreamPool()
{
}

template<typename T>
bool CMemoryStreamPool<T>::Append(T* data, int length)
{
	int ret = this->WriteBuffer(pos, data, length);
	if (true == ret)
		pos += length;
	return ret;
}

template<typename T>
int CMemoryStreamPool<T>::GetPosition()
{
	return pos;
}
