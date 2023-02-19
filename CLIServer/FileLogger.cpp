#include "FileLogger.h"

CFileLogger::CFileLogger(std::string strFileName)
{
	m_filename = strFileName;
}

CFileLogger::~CFileLogger()
{
}

void CFileLogger::LogWrite(std::string function, int line, std::string message)
{
}
