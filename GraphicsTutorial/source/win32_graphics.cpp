#include <windows.h>

#include "win32_graphics.h"

global_state global_state;

LRESULT Win32WindowCallback(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT result = {};
    switch (Message)
    {
    case WM_DESTROY:
    case WM_CLOSE:
        {
            global_state.b_is_running = false;
        }
        break;
    default:
        {
            result = DefWindowProcA(WindowHandle, Message, WParam, LParam);
        }
        break;
    }
    return result;
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    global_state.b_is_running = true;

    WNDCLASSA WindowsClass = {};
    WindowsClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowsClass.lpfnWndProc = Win32WindowCallback;
    WindowsClass.hInstance = hInstance;
    WindowsClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    WindowsClass.lpszClassName = "GraphicsTutorialWindowClass";

    if (!RegisterClassA(&WindowsClass))
    {
        MessageBoxA(nullptr, "Window registration failed", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    global_state.window_handle = CreateWindowExA(
        0,
        WindowsClass.lpszClassName,
        "Graphics Tutorial",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        nullptr,
        nullptr,
        hInstance,
        nullptr);

    if (!global_state.window_handle)
    {
        MessageBoxA(nullptr, "Window creation failed", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    while (global_state.b_is_running)
    {
        MSG message = {};
        while (PeekMessageA(&message, global_state.window_handle, 0, 0, PM_REMOVE))
        {
            switch (message.message)
            {
            case WM_QUIT:
                {
                    global_state.b_is_running = false;
                }
                break;
            default:
                {
                    TranslateMessage(&message);
                    DispatchMessageA(&message);
                }
                break;
            }
        }
    }
    return 0;
}
