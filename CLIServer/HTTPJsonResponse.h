#pragma once
#include <json/json.h>
#pragma hdrstop
#include "HTTPResponse.h"


class CHTTPJsonResponse : public IHTTPResponse
{
public:
	CHTTPJsonResponse(Json::Value body);
public:
	std::string GetResponseBody();

private:
	Json::Value body;
};
