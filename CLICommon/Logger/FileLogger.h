#pragma once
#include <fstream>
#pragma hdrstop

#include "Logger.h"

class CFileLogger : public ILogger
{
public:
	CFileLogger(std::string strFileName);
	~CFileLogger();

public:
	void LogWrite(std::string function, int line, std::string message);

private:
	std::ofstream file;
};

