#pragma once
#include <vector>
#include <string>
#include <array>
#include <iterator>
#include <algorithm>
#pragma hdrstop

#include "ProtocolID.h"

class CProtocolProvider
{
public:
	static std::vector<unsigned char> GetPacket_CreateFile(std::string filepath);
	static std::vector<unsigned char> GetPacket_WriteFile(unsigned long long filesize);
	static std::vector<unsigned char> GetPacket_CloseFile();
	static std::vector<unsigned char> GetPacket_HeaderBeat();
	static std::vector<unsigned char> GetPacket_Echo(std::string message);
};

