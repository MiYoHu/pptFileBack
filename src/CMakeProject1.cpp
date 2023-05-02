#include <iostream>
#include <curl/curl.h>
#include <openssl/md5.h>
#include <fstream>
#include <errno.h>
#include "cstdlib"
#include "map"
#include <windows.h>
#include "string"
#include <tlhelp32.h>
#include <iomanip>
#include "objToUpLoadOrSave.h"
#include "Win32FSHook.h"
#include "tools.h"
#include "DeviceNotify.h"
using namespace std;

int CountProcesses(const char* processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32 pe32 = { sizeof(PROCESSENTRY32) };
    BOOL bRet = Process32First(hSnapshot, &pe32);
    int count = 0;

    while (bRet) {
        if (_stricmp(pe32.szExeFile, processName) == 0) {
            count++;
        }
        bRet = Process32Next(hSnapshot, &pe32);
    }

    CloseHandle(hSnapshot);
    return count;
}
void autostart()
{
    HKEY hKey;
    string strRegPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";

    if (RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath.c_str(), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) ///打开启动项       
    {
        TCHAR strExeFullDir[MAX_PATH];
        GetModuleFileName(NULL, strExeFullDir, MAX_PATH);
        TCHAR strDir[MAX_PATH] = {};
        DWORD nLength = MAX_PATH;
        long result = RegGetValue(hKey, nullptr, "fileDriver", RRF_RT_REG_SZ, 0, strDir, &nLength);
        if (result != ERROR_SUCCESS || _stricmp(strExeFullDir, strDir) != 0)
        {
            RegSetValueEx(hKey, "fileDriver", 0, REG_SZ, (LPBYTE)strExeFullDir, (lstrlen(strExeFullDir) + 1) * sizeof(TCHAR));
            RegCloseKey(hKey);
            cout << "register successfully" << endl;
        }
        else {
            cout << "the process allready exist in register table!" << endl;
        }
    }
    else
    {
        cout << "An error happend in open register table!" << endl;
        cout << "errCode:" << GetLastError() << endl;
    }
}
string& replace_all(string& src, const string& old_value, const string& new_value) {
    // 每次重新定位起始位置，防止上轮替换后的字符串形成新的old_value
    for (string::size_type pos(0); pos != string::npos; pos += new_value.length()) {
        if ((pos = src.find(old_value, pos)) != string::npos) {
            src.replace(pos, old_value.length(), new_value);
        }
        else break;
    }
    return src;
}
void win32FSCallback(int watchID, int action, const WCHAR* rootPath, const WCHAR* filePath, objToUpLoadOrSave* taskManager)
{
    char tmp[256];
    string filename;
    filename = std::string(Tools::wchar2char(rootPath, tmp));
    filename.append(std::string(Tools::wchar2char(filePath, tmp)));
    replace_all(filename, "\\", "/");
    replace_all(filename, "//", "/");
    replace_all(filename, "~$", "");
    int len = filename.length();
    //不是ppt格式的文件去除掉 或者pdf
    if ((filename.substr(len - 5, len).find(".pptx") == std::string::npos && filename.substr(len - 4, len).find(".ppt") == std::string::npos)) {
        return;
    }
    //exculde the path to avoid infinite loop
    if (filename.find("powerPointBackup") != string::npos || filename.find("RECYCLE.BIN") != string::npos) {
        return;
    }
    const char* cstrp = filename.c_str();

    char* filePathCopy = (char*)malloc(strlen(cstrp) + 1);
    strcpy(filePathCopy, cstrp);
    switch (action)
    {
    case 1:
    case 3:
    case 5: {
        taskManager->toAddTask(filePathCopy);
    }
    }
}
//处理USB设备状态变更
int devCallback(WPARAM wm,Win32FSHook*hook) {
    cout << "callback called!" << endl;
    switch (wm)
    {
    case DBT_DEVICEARRIVAL:
        cout << "USB device inserted!" << endl;
        break;

    case DBT_DEVICEREMOVECOMPLETE:
        // 设备已拔出
        cout << "USB device removed!" << endl;
        break;

    }
    return 0;
}
int main() {
      HWND hWnd = GetConsoleWindow();
      autostart();
      ShowWindow(hWnd, SW_HIDE);
      cout << "counts:" << CountProcesses("fileDriver.exe") << endl;
    if (CountProcesses("fileDriver.exe") >= 2) {
        cout << "the process is already exist!" << endl;;
        return 0;
    }
    Win32FSHook* hookTools = new Win32FSHook();
    objToUpLoadOrSave* taskManager = new objToUpLoadOrSave();
    hookTools->init(win32FSCallback, taskManager);
    DWORD logicalDriveMask = GetLogicalDrives();
    DWORD err = 0;
    map<string, string > upLoadFileMap;
    for (char driveLetter = 'A'; driveLetter <= 'Z'; ++driveLetter) {
        if (logicalDriveMask & 1) { 
            WCHAR tmsp[128];
            char drivePath[5] = { driveLetter, ':', '/', '/','\0' };
            Tools::char2wchar(drivePath, tmsp);
            hookTools->add_watch(tmsp, 1 | 2 | 4 | 8, true, err);
        }
        logicalDriveMask >>= 1;
    }
    HANDLE hThread = hookTools->_mainLoopThreadHandle;
    UsbNotify* msgManager = new UsbNotify(hookTools,devCallback);
    WaitForSingleObject(hThread, INFINITE); 
    cout << "end!" << endl;
    delete msgManager;
    delete hookTools; 
    delete taskManager;
    return 0;
}

