#include "FileLogger.h"

CFileLogger::CFileLogger(std::string strFileName)
{
	file.open(strFileName);
}

CFileLogger::~CFileLogger()
{
	if (true == file.is_open())
		file.close();
}

void CFileLogger::LogWrite(std::string function, int line, std::string message)
{
	file << "[" << function << "(" << line << ")] " << message << std::endl;
}
