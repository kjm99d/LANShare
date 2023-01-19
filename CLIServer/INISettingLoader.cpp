#include "INISettingLoader.h"

bool CINISettingLoader::Get(string key, UINT& dest)
{
	UINT buffer = GetPrivateProfileIntA(m_section.c_str(), key.c_str(), 0, m_ini_path.c_str());
	dest = buffer;

	return true;
}

bool CINISettingLoader::Get(string key, string& dest)
{
	char buffer[MAX_PATH] = { 0, };
	const DWORD nBufferCopied = GetPrivateProfileStringA(m_section.c_str(), key.c_str(), "NONE",
		buffer, MAX_PATH, m_ini_path.c_str());

	dest = buffer;

	if (nBufferCopied > 0)
		return true;
	else
		return false;
}

bool CINISettingLoader::Set(string key, string value)
{
	const BOOL ret = WritePrivateProfileStringA(m_section.c_str(), key.c_str(), value.c_str(), m_ini_path.c_str());
	
	if (0 != ret)
		return true;
	else
		return false;
}
