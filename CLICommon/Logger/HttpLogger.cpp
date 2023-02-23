#include "HttpLogger.h"


CHttpLogger::CHttpLogger(std::string url, std::string message)
{
	m_url = url;
	m_message = message;
}

CHttpLogger::~CHttpLogger()
{
}

void CHttpLogger::LogWrite(std::string function, int line, std::string message)
{
}
