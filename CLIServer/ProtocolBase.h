/*****************************************************************//**
 * \file   ProtocolBase.h
 * \brief  수신된 패킷의 프로토콜 인터페이스
 * 
 * \author KJM
 * \date   January 2023
 *********************************************************************/

#pragma once
#include <string>
#include <vector>

using namespace std;


class CProtocolBase 
{
public:
	virtual void SetMessage(string message) = 0;
	virtual bool Parse() = 0;
	virtual void Start() = 0;

protected:
	string message;
};

