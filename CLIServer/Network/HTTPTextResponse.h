#pragma once
#include <string>
#pragma hdrstop

#include "HTTPResponse.h"


class CHTTPTextResponse : public IHTTPResponse
{
public:
	CHTTPTextResponse(std::string body);
public:
	std::string GetResponseBody();

private:
	std::string body;
};

