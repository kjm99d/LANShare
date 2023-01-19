#pragma once
#include <string>

using namespace std;

class CProtocolBase
{
public:
	CProtocolBase(string name, string hash, long size, char* buffer);
	CProtocolBase() = delete;
	~CProtocolBase();



protected:
	string	m_name;	// 파일명
	string	m_hash;	// 파일의 해쉬 정보 (무결성 체크)
	long	m_size;	// 파일의 길이
	char*	m_buffer; // 버퍼




};

