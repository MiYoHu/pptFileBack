#include "tools.h"
#include <iostream>
#include <openssl/md5.h>
#include <sstream>
#include <iomanip>
char* Tools::wchar2char(const wchar_t* wchar, char* m_char)
{
    int len = WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), NULL, 0, NULL, NULL);
    WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), m_char, len, NULL, NULL);
    m_char[len] = '\0';
    return m_char;
}
wchar_t* Tools::char2wchar(const char* cchar, wchar_t* m_wchar)
{
    int len = MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), NULL, 0);
    MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), m_wchar, len);
    m_wchar[len] = '\0';
    
    return m_wchar;
}
   std::string Tools::ANSItoUTF8(const char* strAnsi)
{
    //获取转换为宽字节后需要的缓冲区大小，创建宽字节缓冲区，936为简体中文GB2312代码页  
    int nLen = MultiByteToWideChar(CP_ACP, NULL, strAnsi, -1, NULL, NULL);
    WCHAR* wszBuffer = new WCHAR[nLen + 1];
    nLen = MultiByteToWideChar(CP_ACP, NULL, strAnsi, -1, wszBuffer, nLen);
    wszBuffer[nLen] = 0;
    //获取转为UTF8多字节后需要的缓冲区大小，创建多字节缓冲区  
    nLen = WideCharToMultiByte(CP_UTF8, NULL, wszBuffer, -1, NULL, NULL, NULL, NULL);
    CHAR* szBuffer = new CHAR[nLen + 1];
    nLen = WideCharToMultiByte(CP_UTF8, NULL, wszBuffer, -1, szBuffer, nLen, NULL, NULL);
    szBuffer[nLen] = 0;

    std::string s1 = szBuffer;
    //内存清理  
    delete[]wszBuffer;
    delete[]szBuffer;

    return s1;
}
   std::string Tools::calcFileSign(const char* filePath) {
       HANDLE fileHandle = CreateFile(
           filePath,                  // 文件路径
           GENERIC_READ,              // 访问模式：读取
           FILE_SHARE_READ,           // 共享模式：允许其他程序只读访问
           NULL,                      // 安全属性：默认值（NULL）
           OPEN_EXISTING,             // 打开方式：只打开已存在的文件
           FILE_ATTRIBUTE_NORMAL,     // 文件属性：默认值
           NULL                       // 模板句柄：没有模板
       );
       if (fileHandle == INVALID_HANDLE_VALUE) {
           std::cerr << "Failed to open file: " << filePath << std::endl;
           std::cerr << "errCode" << GetLastError() << std::endl;
           return "failed";
       }

       MD5_CTX md5Context;
       MD5_Init(&md5Context);

       const int bufSize = 1024;
       unsigned char buf[bufSize];
       DWORD bytesRead = 0;
       while (ReadFile(fileHandle, buf, bufSize, &bytesRead, nullptr)) {
           if (bytesRead == 0) break;
           MD5_Update(&md5Context, buf, bytesRead);
       }

       unsigned char md[MD5_DIGEST_LENGTH];
       MD5_Final(md, &md5Context);

       CloseHandle(fileHandle);
       std::ostringstream oss;

       for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
           oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(md[i]);
       }

       return oss.str();
   }