#pragma once
/*****************************************************************//**
 * \file   SettingLoader.h
 * \brief  세팅파일을 로딩하는 클래스
 * 
 * \author jungmin.kim
 * \date   January 2023
 *********************************************************************/
#include <string>
using namespace std;

class ISettingLoader 
{
protected:
	virtual bool Get(string key, string& dest) = 0;
	virtual bool Set(string key, string value) = 0;
};
