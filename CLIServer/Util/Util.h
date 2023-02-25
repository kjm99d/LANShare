#pragma once
#include <Windows.h>
#include <string>

namespace Util {
	BOOL OpenFileDialog(char* buffer, int length);
	std::string URLDecode(const std::string& src);
}