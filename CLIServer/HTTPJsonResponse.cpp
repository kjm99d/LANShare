#include "HTTPJsonResponse.h"

CHTTPJsonResponse::CHTTPJsonResponse(Json::Value body)
{
	this->body = body;
    this->m_contentType = "application/json";
}

string CHTTPJsonResponse::GetResponseBody()
{
    Json::StreamWriterBuilder builder;
    const std::string json_file = Json::writeString(builder, body);

    return json_file;
}
