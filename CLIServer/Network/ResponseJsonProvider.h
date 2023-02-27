/*****************************************************************//**
 * \file   ResponseJsonProvider.h
 * \brief  LANShare 프로젝트에서 응답시 사용하는 표준 프로토콜 응답데이터 정의
 * 
 * \author KJM
 * \date   February 2023
 *********************************************************************/
#pragma once
#include <json/json.h>
#include <string>
#pragma hdrstop

#include "HTTPResponse.h"

class CResponseJsonProvider : public IHTTPResponse
{
public :
	CResponseJsonProvider(int iResStatusCode, std::string message, Json::Value json);
	std::string GetResponseBody();

private:
	int m_responseStatusCode;
	std::string m_message;
	Json::Value m_body;

};

