#pragma once
#include "Logger.h"
class CFileLogger : public ILogger
{
public:
	CFileLogger(std::string strFileName);
	~CFileLogger();

public:
	void LogWrite(std::string function, int line, std::string message);

private:
	std::string m_filename;
};

