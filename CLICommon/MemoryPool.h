/*****************************************************************//**
 * \file   MemoryPool.h
 * \brief  정적 메모리 크기를 미리 잡아두고, 동적으로 제어하여 동적할당 하는데 드는 코스트를 줄여 속도를 개선하는 클래스
 * \brief  일반적으로 상속을 받아 스트림 형태로 사용한다.
 * \brief  사용시 특정 함수 내에서 반복적인 객체 생성을 방지하기 위해 static 타입으로 사용 해야한다.
 * 
 * 
 * 
 * \author KJM
 * \date   January 2023
 *********************************************************************/
#pragma once
#include <stdlib.h>
#include <string>

#ifdef ZeroMemory
	#undef ZeroMemory
#endif

template <typename T>
class CMemoryPool
{
public:
	CMemoryPool() = delete;
	CMemoryPool(int length);
	~CMemoryPool();

public:
	int Length();
	bool Clear();
	bool ZeroMemory() { Clear(); };
	bool WriteBuffer(int pos, T* buffer, int nLenBuffer);
	const T* const GetBuffer();


private:
	int length;
	T* buffer;
};

// ======================================================================== // 

template <typename T>
CMemoryPool<T>::CMemoryPool(int length)
{
	this->length = length;
	buffer = nullptr;

	// 실패 할 일은 없지만, 행걸리면 암튼 여기임 ㅋ
	do {
		buffer = (char*)malloc(sizeof(T) * length);
		// if (buffer == nullptr) << 그럴리 없을꺼야 .. 응 .. 
	} while (buffer == nullptr);
}

template <typename T>
CMemoryPool<T>::~CMemoryPool()
{
	delete buffer;
	buffer = nullptr;
}

template <typename T>
int CMemoryPool<T>::Length()
{
	return length;
}

template <typename T>
bool CMemoryPool<T>::Clear()
{
	int ret = true;

	memset(buffer, 0x00, sizeof(T) * length);
	if (buffer[0] != 0x00)
		ret = false;


	return ret;

}

template <typename T>
bool CMemoryPool<T>::WriteBuffer(int pos, T* buffer, int nLenBuffer)
{
	bool ret = true;

	// 버퍼에 쓸 위치가 이 버퍼풀의 최대 크기보다 크면 안됭 그럼 곤란행
	const int max = Length();
	if (pos > max)
		ret = false;
	else if (pos + nLenBuffer > max)
		ret = false;
	else
		memcpy(this->buffer + pos, (void *)buffer, sizeof(T) * nLenBuffer);

	return ret;
}

template<typename T>
const T* const CMemoryPool<T>::GetBuffer()
{
	return buffer;
}
