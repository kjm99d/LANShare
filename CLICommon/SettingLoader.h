#pragma once
/*****************************************************************//**
 * \file   SettingLoader.h
 * \brief  세팅파일을 로딩하는 클래스
 * 
 * \author jungmin.kim
 * \date   January 2023
 *********************************************************************/
#include <string >


class ISettingLoader 
{
protected:
	virtual bool Get( std::string  key, UINT& dest) = 0;
	virtual bool Get( std::string  key,  std::string & dest) = 0;
	virtual bool Set( std::string  key,  std::string  value) = 0;
};
