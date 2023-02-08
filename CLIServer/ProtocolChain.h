/*****************************************************************//**
 * \file   ProtocolBuilder.h
 * \brief  클래스 체인형태로 메세지를 작성하여 프로토콜 체인을 구성해주는 클래스
 * 
 * \author KJM
 * \date   February 2023
 *********************************************************************/
#pragma once

#include <string>
#include <vector>
#include <stdint.h>
#pragma hdrstop

class CProtocolChain
{
public:
	CProtocolChain() = delete;
	CProtocolChain(int iCommand) {
		// --------------- // 전체길이 + Command 크기
		packet.resize(sizeof(uintmax_t) + 4);
		std::copy((unsigned char*)&iCommand, ((unsigned char*)&iCommand) + sizeof(int), packet.begin() + sizeof(uintmax_t));
	}
public:
	// 값은 무조건 한번만 설정이 가능하게한다.
	CProtocolChain& SetMessage(std::string message) 
	{ 
		int nPosData = packet.size();

		packet.resize(nPosData + message.length());
		std::copy(message.begin(), message.end(), packet.begin() + nPosData);

		nPosData = packet.size();
		std::copy(reinterpret_cast<unsigned char *>(&nPosData), reinterpret_cast<unsigned char*>(&nPosData) + sizeof(nPosData), packet.begin());

		return *this;
	}

	CProtocolChain& SetData(uintmax_t length)
	{ 
		int nPosData = packet.size();
		packet.resize(nPosData + sizeof(uintmax_t)); // 늘리고

		// 데이터 넣고
		std::copy(reinterpret_cast<unsigned char*>(&length), reinterpret_cast<unsigned char*>(&length) + sizeof(uintmax_t), packet.begin() + nPosData);

		nPosData = packet.size(); // 최종길이 수정
		std::copy(reinterpret_cast<unsigned char*>(&nPosData), reinterpret_cast<unsigned char*>(&nPosData) + sizeof(nPosData), packet.begin());

		return *this;
	}

	const std::vector<unsigned char> Build()
	{
		return packet;
	}


private:
	std::vector<unsigned char> packet;
};

