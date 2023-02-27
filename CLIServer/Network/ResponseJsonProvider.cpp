#include "ResponseJsonProvider.h"

CResponseJsonProvider::CResponseJsonProvider(int ResStatusCode, std::string message, Json::Value json)
{
	this->m_responseStatusCode = ResStatusCode;
	this->m_message = message;
	this->m_body = json;
	this->m_contentType = "application/json";
}

std::string CResponseJsonProvider::GetResponseBody()
{
	Json::Value response;

	response["status_code"] = m_responseStatusCode;
	response["message"] = m_message;
	response["data"] = m_body;

	Json::StreamWriterBuilder builder;
	const std::string json_file = Json::writeString(builder, response);

	return json_file;

}
