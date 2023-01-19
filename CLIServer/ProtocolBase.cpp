#include "ProtocolBase.h"

// =========================================================================================== //
CProtocolBase::CProtocolBase(string name, string hash, long size, char* buffer)
{
	m_name = name;
	m_hash = hash;
	m_size = size;
	m_buffer = buffer; // NULL 저장 가능
}

CProtocolBase::~CProtocolBase()
{
}
// =========================================================================================== //