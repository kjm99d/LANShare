#pragma once
#include <string>
#include <Windows.h>

#include "SettingLoader.h"

/**
 * \brief INI포맷의 파일을 조회 및 구축 하는 클래스.
 */
class CINISettingLoader : public ISettingLoader
{
public:
	CINISettingLoader(std::string ini_path) { m_ini_path = ini_path; };
	~CINISettingLoader() {} ;
	

public:
	void SetSection(std::string section) { m_section = section; };

	bool Get(std::string key, UINT& dest)	override;
	bool Get(std::string key, std::string& dest)	override;
	bool Set(std::string key, std::string value)	override;


private:
	std::string m_section;
	std::string m_ini_path;
};

