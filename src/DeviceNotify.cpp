#include "DeviceNotify.h"
#include <iostream>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UsbNotify* pThis = (UsbNotify*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    switch (uMsg)
    {
    case WM_DEVICECHANGE:
        // 检查设备接口更改消息类型
        if (pThis->_callback) {
            pThis->_callback(wParam,pThis->winHooks);
        }
        break;

    case WM_DESTROY:
        // 注销设备接口通知并退出消息循环
        UnregisterDeviceNotification(hwnd);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

UsbNotify::UsbNotify(Win32FSHook* hook, devChangeCallback back){
    winHooks = hook;
    _callback = back;
    DWORD dwThreadId;
    auto dwThrdParam = (LPVOID)this;
    msgThread_ = CreateThread(
        NULL,
        0,
        UsbNotify::getMsg,
        dwThrdParam,
        0,
        &dwThreadId);
}

DWORD __stdcall UsbNotify::getMsg(LPVOID lpParam)
{
    UsbNotify* _this = (UsbNotify*)lpParam;
    HINSTANCE hInst = GetModuleHandle(NULL);
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInst;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "MainWindowClass";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        std::cout << "errCode is :" << GetLastError() << std::endl;
    }

    _this->hwndMain = CreateWindowEx(
        0,
        "MainWindowClass",  
        "My Window",  
        WS_OVERLAPPEDWINDOW, 
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );
    if (_this->hwndMain == INVALID_HANDLE_VALUE) {
        cout << "error to create handle" << endl;
        return 0;
    }
    SetWindowLongPtr(_this->hwndMain, GWLP_USERDATA, (LONG_PTR)_this);
    DEV_BROADCAST_DEVICEINTERFACE notifyFilter;
    notifyFilter.dbcc_size = sizeof(notifyFilter);
    notifyFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    notifyFilter.dbcc_classguid = GUID_DEVINTERFACE_USB_DEVICE;
    HDEVNOTIFY hDevNotify = RegisterDeviceNotification(
        _this->hwndMain,
        &notifyFilter,
        DEVICE_NOTIFY_WINDOW_HANDLE
    );
    if (hDevNotify == NULL)
    {
        DWORD error = GetLastError();
        std::cerr << "Failed to register device notification. Error code: " << error << std::endl;
    }
    else
    {
        std::cout << "Device notification registered successfully." << std::endl;
    }
    cout << "start monitor; " << endl;
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        cout << "received msg!" << endl;
        TranslateMessage(&msg);
        DispatchMessage(&msg);   // 将消息发送至窗口过程函数进行处理
    }
    return 0;
}
