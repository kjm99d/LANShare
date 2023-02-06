#pragma once
#include <string>

using namespace std;
#pragma hdrstop

#include "HTTPResponse.h"


class CHTTPTextResponse : public IHTTPResponse
{
public:
	CHTTPTextResponse(string body);
public:
	string GetResponseBody();

private:
	string body;
};

