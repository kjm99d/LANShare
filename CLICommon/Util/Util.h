#pragma once
#include <Windows.h>
#include <string>
#include <filesystem>
#include <fstream>

namespace Util {
	BOOL OpenFileDialog(char* buffer, int length);

	bool CreateFileLock(std::string strLockFileName);

	std::string URLDecode(const std::string& src);
}