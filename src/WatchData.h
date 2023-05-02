//
// Created by ÚØ³¾ on 2023-04-22.
//

#ifndef UNTITLED1_WATCHDATA_H
#define UNTITLED1_WATCHDATA_H


#include <windows.h>
#include <winbase.h>
#include <winnt.h>
#include <string>
#include "objToUpLoadOrSave.h"
using namespace std;

typedef void(*ChangeCallback)(int watchID, int action, const WCHAR* rootPath, const WCHAR* filePath,objToUpLoadOrSave* obj);

class WatchData
{
private:
    static int _counter;
    WCHAR* _path;
    int _watchId;
    HANDLE _hDir;
    int _mask;
    bool _watchSubtree;
    DWORD _byteReturned;
    OVERLAPPED _overLapped;
    char _buffer[8196];
    HANDLE _completionPort;
public:
    WatchData();
    WatchData(const WCHAR* path, int mask, bool watchSubtree, HANDLE completionPort);
    virtual ~WatchData();

    const char* getBuffer(){ return _buffer;}
//	FILE_NOTIFY_INFORMATION* getNotifyInfo(){return _notifyInfo;}
    const int getBufferSize() {return sizeof(_buffer);}
//	const DWORD getBytesReturned() {return _byteReturned;}
    const WCHAR* getPath() {return _path;}
    const int getId() {return _watchId;}
//	const HANDLE getDirHandle() {return _hDir;}
//	const int getMask() {return _mask;}
    int watchDirectory();

    // cancel pending watch on the hDir, returns 0 if okay or errorCode otherwise.
    int unwatchDirectory();
};



#endif //UNTITLED1_WATCHDATA_H
