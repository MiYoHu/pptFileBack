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
    //��ȡת��Ϊ���ֽں���Ҫ�Ļ�������С���������ֽڻ�������936Ϊ��������GB2312����ҳ  
    int nLen = MultiByteToWideChar(CP_ACP, NULL, strAnsi, -1, NULL, NULL);
    WCHAR* wszBuffer = new WCHAR[nLen + 1];
    nLen = MultiByteToWideChar(CP_ACP, NULL, strAnsi, -1, wszBuffer, nLen);
    wszBuffer[nLen] = 0;
    //��ȡתΪUTF8���ֽں���Ҫ�Ļ�������С���������ֽڻ�����  
    nLen = WideCharToMultiByte(CP_UTF8, NULL, wszBuffer, -1, NULL, NULL, NULL, NULL);
    CHAR* szBuffer = new CHAR[nLen + 1];
    nLen = WideCharToMultiByte(CP_UTF8, NULL, wszBuffer, -1, szBuffer, nLen, NULL, NULL);
    szBuffer[nLen] = 0;

    std::string s1 = szBuffer;
    //�ڴ�����  
    delete[]wszBuffer;
    delete[]szBuffer;

    return s1;
}
   std::string Tools::calcFileSign(const char* filePath) {
       HANDLE fileHandle = CreateFile(
           filePath,                  // �ļ�·��
           GENERIC_READ,              // ����ģʽ����ȡ
           FILE_SHARE_READ,           // ����ģʽ��������������ֻ������
           NULL,                      // ��ȫ���ԣ�Ĭ��ֵ��NULL��
           OPEN_EXISTING,             // �򿪷�ʽ��ֻ���Ѵ��ڵ��ļ�
           FILE_ATTRIBUTE_NORMAL,     // �ļ����ԣ�Ĭ��ֵ
           NULL                       // ģ������û��ģ��
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