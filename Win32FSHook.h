//
// Created by ÚØ³¾ on 2023-04-22.
//

#ifndef UNTITLED1_WIN32FSHOOK_H
#define UNTITLED1_WIN32FSHOOK_H




#include <windows.h>
#include <string>
#include <map>
#include <queue>
#include <utility>


#include "WatchData.h"
#include "objToUpLoadOrSave.h"
using namespace std;

class Event
{
public:
    int _watchID;
    int _action;
    WCHAR* _rootPath;
    WCHAR* _filePath;
    Event(int wd, int action, const WCHAR* rootPath, const WCHAR* filePath)
    {
        _watchID = wd;
        _action = action;
        size_t len1 = wcslen(rootPath);
        size_t len2 = wcslen(filePath);
        _rootPath = new WCHAR[len1 + 1];
        _filePath = new WCHAR[len2 + 1];
        wcsncpy(_rootPath, rootPath, len1);
        _rootPath[len1] = 0;
        wcsncpy(_filePath, filePath, len2);
        _filePath[len2] = 0;
    }

    ~Event()
    {
        delete [] _rootPath;
        delete [] _filePath;
    }
};
class Win32FSHook
{
private:
    static const DWORD DELETE_WD_SIGNAL = 99999997;
    static const DWORD EXIT_SIGNAL = 99999998;
    static Win32FSHook *instance;

    // running flag
    bool _isRunning;

    HANDLE _completionPort;

    // critical seaction
    CRITICAL_SECTION _cSection;

    // watch id 2 watch map
    map<int, WatchData*> _wid2WatchData;
    objToUpLoadOrSave *taskHandle;
    // Thread function
    static DWORD WINAPI mainLoop( LPVOID lpParam );

    ChangeCallback _callback;

    void watchDirectory(WatchData* wd);

    CRITICAL_SECTION _eventQueueLock;
    HANDLE _eventQueueEvent;
    void postEvent(Event *event);
    HANDLE _eventsThread;
    static DWORD WINAPI eventLoop( LPVOID lpParam );
    queue<Event*> _eventQueue;
    WatchData* find(int wd);
public:
    static const int ERR_INIT_THREAD = 1;

    Win32FSHook();
    virtual ~Win32FSHook();

    void init(ChangeCallback callback,objToUpLoadOrSave* taskManager);

    int add_watch(const WCHAR* path, long notifyFilter, bool watchSubdirs, DWORD &error);
    void remove_watch(int watchId);

// thread handle
HANDLE _mainLoopThreadHandle;
};


#endif //UNTITLED1_WIN32FSHOOK_H
