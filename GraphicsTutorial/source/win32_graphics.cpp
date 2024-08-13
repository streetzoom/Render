#include <windows.h>

#include "win32_graphics.h"

#include <cassert>

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
    LARGE_INTEGER perf_count_frequency_result = {};
    assert(QueryPerformanceFrequency(&perf_count_frequency_result));

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
        1280,
        720,
        nullptr,
        nullptr,
        hInstance,
        nullptr);

    if (!global_state.window_handle)
    {
        MessageBoxA(nullptr, "Window creation failed", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    global_state.device_context = GetDC(global_state.window_handle);

    RECT client_rect {};
    assert(GetClientRect(global_state.window_handle, &client_rect));
    global_state.frame_buffer_width = client_rect.right - client_rect.left;
    global_state.frame_buffer_height = client_rect.bottom - client_rect.top;
    global_state.frame_buffer_pixels = new UINT32[global_state.frame_buffer_width * global_state.frame_buffer_height];

    LARGE_INTEGER begin_time = {};
    LARGE_INTEGER end_time = {};
    QueryPerformanceCounter(&begin_time);
    
    while (global_state.b_is_running)
    {
        assert(QueryPerformanceCounter(&end_time));
        float delta_time = static_cast<float>(end_time.QuadPart - begin_time.QuadPart) / perf_count_frequency_result.QuadPart;
        begin_time = end_time;
        
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

        constexpr float offset_speed = 165.0f;
        global_state.current_offset += offset_speed * delta_time;
        
        for (UINT32 y = 0; y < global_state.frame_buffer_height; ++y)
        {
            for (UINT32 x = 0; x < global_state.frame_buffer_width; ++x)
            {
                global_state.frame_buffer_pixels[y * global_state.frame_buffer_width + x] = 0xFF0000FF;
                const UINT8 red = static_cast<UINT8>(x * 255 / global_state.frame_buffer_width - global_state.current_offset);
                const UINT8 green = static_cast<UINT8>(y * 255 / global_state.frame_buffer_height);
                constexpr UINT8 blue = 0;
                constexpr UINT8 alpha = 255;
                global_state.frame_buffer_pixels[y * global_state.frame_buffer_width + x] = (alpha << 32) | (red << 16) | (green << 8 ) | (blue << 4);
            }
        }

        RECT client_rect {};
        assert(GetClientRect(global_state.window_handle, &client_rect));

        global_state.frame_buffer_width = client_rect.right - client_rect.left;
        global_state.frame_buffer_height = client_rect.bottom - client_rect.top;

        BITMAPINFO bitmap_info = {};
        bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
        bitmap_info.bmiHeader.biWidth = global_state.frame_buffer_width;
        bitmap_info.bmiHeader.biHeight = global_state.frame_buffer_height;
        bitmap_info.bmiHeader.biPlanes = 1;
        bitmap_info.bmiHeader.biBitCount = 32;
        bitmap_info.bmiHeader.biCompression = BI_RGB;

        StretchDIBits(global_state.device_context, 0, 0, global_state.frame_buffer_width, global_state.frame_buffer_height, 0, 0, global_state.frame_buffer_width, global_state.frame_buffer_height, global_state.frame_buffer_pixels, &bitmap_info, DIB_RGB_COLORS, SRCCOPY);
    }
    return 0;
}
