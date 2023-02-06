#include "HTTPTextResponse.h"

CHTTPTextResponse::CHTTPTextResponse(string body)
{
	this->body = body;
	this->m_contentType = "text/html";
}

string CHTTPTextResponse::GetResponseBody()
{
	return body;
}
