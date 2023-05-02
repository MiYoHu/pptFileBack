//
// Created by ÚØ³¾ on 2023-04-24.
//

#ifndef UNTITLED1_OBJTOUPLOADORSAVE_H
#define UNTITLED1_OBJTOUPLOADORSAVE_H
#include "windows.h"
#include "map"
class objToUpLoadOrSave {
    HANDLE _eventEvent;
    CRITICAL_SECTION _mapLocke;
    std::string AskUrl;
    std::string upLoadUrl;
    std::map <std::string ,const char *> taskMap;
    HANDLE _eventsThread;
    HANDLE _upLoadThread;
public:
    void toAddTask(const char * _path);
    static DWORD WINAPI  toUpLoad( LPVOID lpParam );
    static DWORD WINAPI  TrackEverything(LPVOID lpParam);
    objToUpLoadOrSave();
};


#endif //UNTITLED1_OBJTOUPLOADORSAVE_H
