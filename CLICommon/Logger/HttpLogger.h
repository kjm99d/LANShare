#pragma once
#include "Logger.h"

class CHttpLogger : public ILogger
{
public:
	CHttpLogger(std::string url, std::string message);
	~CHttpLogger();

public:
	void LogWrite(std::string function, int line, std::string message);


private:
	std::string m_url;
	std::string m_message;
};

