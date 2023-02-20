#pragma once
#include "Logger.h"
#include <iostream>

class CConsoleLogger : public ILogger
{
public:
	CConsoleLogger();
	~CConsoleLogger();

public:
	void LogWrite(std::string function, int line, std::string message);
};

