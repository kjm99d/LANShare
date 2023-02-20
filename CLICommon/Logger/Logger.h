#pragma once
#include <string>

#define LOG_WRITE(message)	LogWrite(__FUNCTION__, __LINE__, message);


class ILogger
{
public:
	virtual void LogWrite(std::string function, int line, std::string message) = 0;
	// virtual void LogWrite(std::wstring function, int line, std::wstring message) = 0;
};

