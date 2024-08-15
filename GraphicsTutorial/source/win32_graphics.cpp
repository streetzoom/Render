#include <cmath>
#include <windows.h>

#include "win32_graphics.h"

#include <cassert>
#include "graphics_math.h"

global_state global_state;

vector2d ProjectPoint(vector3d WorldPosition)
{
    vector2d Result = WorldPosition.xy / WorldPosition.z;
    Result = 0.5f * (Result + Vector_2d(1.0f, 1.0f));
    return Result;
}


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

    global_state.frame_buffer_width = 300;
    global_state.frame_buffer_height = 300;
    global_state.frame_buffer_pixels = new uint32_t[global_state.frame_buffer_width * global_state.frame_buffer_height];

    LARGE_INTEGER begin_time = {};
    LARGE_INTEGER end_time = {};
    QueryPerformanceCounter(&begin_time);

    while (global_state.b_is_running)
    {
        assert(QueryPerformanceCounter(&end_time));
        float delta_time = static_cast<float>(end_time.QuadPart - begin_time.QuadPart) / perf_count_frequency_result.
            QuadPart;
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

        for (UINT32 y = 0; y < global_state.frame_buffer_height; ++y)
        {
            for (UINT32 x = 0; x < global_state.frame_buffer_width; ++x)
            {
                uint32_t PixelId = y * global_state.frame_buffer_width + x;

                uint8_t Red = 0;
                uint8_t Green = 0;
                uint8_t Blue = 0;
                uint8_t Alpha = 255;
                uint32_t PixelColor = ((uint32_t)Alpha << 24) | ((uint32_t)Red << 16) | ((uint32_t)Green << 8) | (
                    uint32_t)Blue;

                global_state.frame_buffer_pixels[PixelId] = PixelColor;
            }
        }

        global_state.current_time = global_state.current_time + delta_time;
        if (global_state.current_time > 2.0f * PI)
        {
            global_state.current_time -= 2.0f * PI;
        }

        for (uint32_t TriangleId = 0; TriangleId < 10; ++TriangleId)
        {
            float DistToCamera = powf(2.0f, TriangleId + 1);
            vector3d Points[3] =
            {
                Vector_3d(-1.0f, -0.5f, DistToCamera),
                Vector_3d(1.0f, -0.5f, DistToCamera),
                Vector_3d(0, 0.5f, DistToCamera),
            };

            for (uint32_t PointId = 0; PointId < std::size(Points); ++PointId)
            {
                vector3d ShiftedPoint = Points[PointId] + Vector_3d(cosf(global_state.current_time),
                                                                    sinf(global_state.current_time), 0);
                vector2d PixelPos = ProjectPoint(ShiftedPoint);

                if (PixelPos.x >= 0.0f && PixelPos.x < global_state.frame_buffer_width &&
                    PixelPos.y >= 0.0f && PixelPos.y < global_state.frame_buffer_height)
                {
                    uint32_t PixelId = uint32_t(PixelPos.y) * global_state.frame_buffer_width + uint32_t(PixelPos.x);
                    global_state.frame_buffer_pixels[PixelId] = 0xFF00FF00;
                }
            }
        }

        RECT client_rect{};
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

        StretchDIBits(global_state.device_context, 0, 0, global_state.frame_buffer_width,
                      global_state.frame_buffer_height, 0, 0, global_state.frame_buffer_width,
                      global_state.frame_buffer_height, global_state.frame_buffer_pixels, &bitmap_info, DIB_RGB_COLORS,
                      SRCCOPY);
    }
    return 0;
}
