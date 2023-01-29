/*****************************************************************//**
 * \file   PakcetReader.h
 * \brief  패킷을 받아, 해당 패킷의 데이터를 파싱하는 클래스.
 * 
 * \author KJM
 * \date   January 2023
 *********************************************************************/
#pragma once
class CPakcetReader
{
public:
	CPakcetReader() = delete;
	CPakcetReader(CPakcetReader&) = delete;

	CPakcetReader(char * buffer, int length);
	~CPakcetReader();

public:
	int cmd;
	int length;

};

