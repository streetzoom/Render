#pragma once

struct global_state
{
    HWND window_handle;
    bool b_is_running;
    HDC device_context;

    uint32_t frame_buffer_width;
    uint32_t frame_buffer_height;
    uint32_t* frame_buffer_pixels;
    float current_time;
};

class win32_graphics
{
public: 

};
