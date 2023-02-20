#include "ConsoleLogger.h"

CConsoleLogger::CConsoleLogger()
{
}

CConsoleLogger::~CConsoleLogger()
{
}

void CConsoleLogger::LogWrite(std::string function, int line, std::string message)
{
	std::cout << "[" << function << "(" << line << ")] " << message << std::endl;
}
