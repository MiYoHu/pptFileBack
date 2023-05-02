#ifndef TOOLS_H_
#define TOOLS_H_
#include <windows.h>
#include <string>
class Tools
{
public:
	static char* wchar2char(const wchar_t* wchar, char* m_char);
	static wchar_t* char2wchar(const char* cchar, wchar_t* m_wchar);
	static std::string ANSItoUTF8(const char* strAnsi);
	static std::string calcFileSign(const char* filePath);
};

#endif // !TOOLS_H_
