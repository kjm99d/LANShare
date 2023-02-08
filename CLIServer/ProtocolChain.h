/*****************************************************************//**
 * \file   ProtocolBuilder.h
 * \brief  프로토콜을 만들어주는 프로토콜 빌더
 * 
 * \author KJM
 * \date   February 2023
 *********************************************************************/
#pragma once

#include <string>
#include <vector>
#pragma hdrstop


class CProtocolChain
{
public:
	CProtocolChain() = delete;
	CProtocolChain(int iCommand) {
		packet.resize(4 + 4);
		std::copy((unsigned char*)&iCommand, ((unsigned char*)&iCommand) + sizeof(int), packet.begin() + sizeof(int));
	}
public:
	// 값은 무조건 한번만 설정이 가능하게한다.
	CProtocolChain& SetMessage(std::string message) 
	{ 
		int nPosData = packet.size();

		packet.resize(nPosData + message.length());
		std::copy(message.begin(), message.end(), packet.begin() + nPosData);

		nPosData = packet.size();
		std::copy(reinterpret_cast<unsigned char *>(&nPosData), reinterpret_cast<unsigned char*>(&nPosData) + 4, packet.begin());

		return *this;
	}

	CProtocolChain& SetData(int length)
	{ 
		int nPosData = packet.size();
		packet.resize(nPosData + sizeof(length)); // 늘리고

		// 데이터 넣고
		std::copy(reinterpret_cast<unsigned char*>(&length), reinterpret_cast<unsigned char*>(&length) + 4, packet.begin() + nPosData);

		nPosData = packet.size(); // 최종길이 수정
		std::copy(reinterpret_cast<unsigned char*>(&nPosData), reinterpret_cast<unsigned char*>(&nPosData) + 4, packet.begin());

		return *this;
	}

	const std::vector<unsigned char> Build()
	{
		return packet;
	}


private:
	std::vector<unsigned char> packet;
};

