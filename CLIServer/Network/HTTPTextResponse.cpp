#include "HTTPTextResponse.h"

CHTTPTextResponse::CHTTPTextResponse(std::string body)
{
	this->body = body;
	this->m_contentType = "text/html";
}

std::string CHTTPTextResponse::GetResponseBody()
{
	return body;
}
