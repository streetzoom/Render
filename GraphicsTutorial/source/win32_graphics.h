#pragma once
#include <windows.h>

struct global_state
{
    HWND window_handle;
    bool b_is_running;
    HDC device_context;

    UINT32 frame_buffer_width;
    UINT32 frame_buffer_height;
    UINT32* frame_buffer_pixels;

    float current_offset;
};

class win32_graphics
{
public: 

};
