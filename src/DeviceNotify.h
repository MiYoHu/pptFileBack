#ifndef DEVICE_NOTIFY_H_
#define DEVICE_NOTIFY_H_
#include "Win32FSHook.h"
#include <dbt.h>
#include <winuser.h>
#include <Windows.h>
#include <initguid.h>
#include <Usbiodef.h>
typedef int (*devChangeCallback)(WPARAM notifyType,Win32FSHook*);
class UsbNotify
{
public:
	UsbNotify(Win32FSHook*, devChangeCallback);
	devChangeCallback _callback;
	Win32FSHook* winHooks;
private:
	HANDLE msgThread_;
	HWND hwndMain;
	static DWORD WINAPI getMsg(LPVOID lpParam);
};

#endif // !DEVICE_NOTIFY_H_

