#pragma once
#include <string>
#include <Windows.h>
using namespace std;

#include "SettingLoader.h"
class CINISettingLoader : public ISettingLoader
{
public:
	CINISettingLoader(string ini_path) { m_ini_path = ini_path; };
	~CINISettingLoader() {} ;
	

public:
	void SetSection(string section) { m_section = section; };
	bool Get(string key, string& dest) override;
	bool Set(string key, string value) override;


private:
	string m_section;
	string m_ini_path;
};

