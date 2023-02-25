#include "Util.h"

/**
 * 파일 다이얼로그를 열어, 매개변수로 받아온 메모리 공간에 데이터를 채워넣는다.
 * 
 * \param buffer - 위치 정보가 채워질 버퍼
 * \param length - 위 버퍼의 최대 길이
 * \return 성공(TRUE), 실패(FALSE)
 */
BOOL Util::OpenFileDialog(char* buffer, int length)
{
	OPENFILENAMEA ofn;       // common dialog box structure

	// Initialize OPENFILENAME
	//ZeroMemory(&ofn, sizeof(ofn));
	memset(&ofn, 0x00, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = buffer;
	ofn.nMaxFile = length;
	ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	const BOOL ret = GetOpenFileNameA(&ofn);
	return ret;
}

/**
 * URL 인코딩 된 문자열을 디코딩하여 원문을 구한다.
 *
 * \param src - 인코딩 된 문자열
 * \return
 */
std::string Util::URLDecode(const std::string& src)
{
	std::string ret;
	char ch;
	int i, ii;
	for (i = 0; i < src.length(); i++) 
	{
		if (src[i] == '%') 
		{
			sscanf_s(src.substr(i + 1, 2).c_str(), "%x", &ii);
			ch = static_cast<char>(ii);
			ret += ch;
			i = i + 2;
		}
		else 
		{
			ret += src[i];
		}
	}

	return ret;
}
