#pragma once
#include <vector>
#include <string>
#include <array>
#include <iterator>
#include <algorithm>
#pragma hdrstop

#include "ProtocolID.h"
#include "ProtocolChain.h"

class CProtocolProvider
{
public:
	static std::vector<unsigned char> GetPacket_CreateFile(std::string filepath);
	static std::vector<unsigned char> GetPacket_WriteFile(uintmax_t filesize);
	static std::vector<unsigned char> GetPacket_CloseFile();
	static std::vector<unsigned char> GetPacket_HeaderBeat();
	static std::vector<unsigned char> GetPacket_Echo(std::string message);
	static std::vector<unsigned char> GetPacket_CommandLine(std::string command);
};

