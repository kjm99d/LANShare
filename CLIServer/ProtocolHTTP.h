/*****************************************************************//**
 * \file   ProtocolV1.h
 * \brief  
 * 
 * \author KJM
 * \date   January 2023
 *********************************************************************/
#pragma once
#include <sstream>
#include <vector>

#include "ProtocolBase.h"


using namespace std;

class CProtocolHTTP : public CProtocolBase
{
private:
	string protocol;
	string resource;
	string querystring;
	string method;


public:
	void SetMessage(string message) override;
	bool Parse() override;
	void Start() override;

private:
	const vector<string> streamTokenizer(string message, char delimeter);
};

