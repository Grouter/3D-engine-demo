//#define RELEASE_MODE

#if defined(RELEASE_MODE)
    #define NDEBUG
#else

#endif

#define TINYOBJLOADER_IMPLEMENTATION

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC

#define STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC

// @Todo: check if RVO is happening

#include <windows.h>
#include <Windowsx.h>
#include <glew.h>
#include <wglew.h>
#include <gl/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <math.h>
#include <cmath>
#include <assert.h>
#include <algorithm>    // @Todo: implement own sort

#include <tiny_obj_loader.h>
#include <stb_image.h>
#include <stb_rect_pack.h>
#include <stb_truetype.h>

#include "platform.h"
#include "opengl.cpp"

global HDC   window_context;
global HGLRC opengl_context;

const u32 INITIAL_WINDOW_W = 1920;
const u32 INITIAL_WINDOW_H = 1080;

const u32 VIRTUAL_WINDOW_W = 1920;
const u32 VIRTUAL_WINDOW_H = 1080;

const u32 VIRTUAL_WINDOW_W_2D = 32;
const u32 VIRTUAL_WINDOW_H_2D = 18;

const u32 TARGET_ASPECT_W = 16;
const u32 TARGET_ASPECT_H = 9;

#include "array.h"
#include "bucket_array.h"
#include "math/math.h"
#include "math/vector.h"
#include "math/matrix.h"
#include "math/quaternion.h"
#include "catalog.h"
#include "parse_utils.cpp"
#include "color.h"
#include "utils.h"

#include "graphics.h"
#include "font.h"
#include "animation.h"
#include "resources.h"
#include "hotload.h"
#include "camera.h"
#include "input.h"
#include "render.h"
#include "text_input.h"
#include "entity.h"
#include "console.h"
#include "game.h"
#include "commands.h"

struct Viewport {
    i32 left, bottom;
    i32 width, height;
};

struct GameState {
    // Window and rendering
    u32 window_width;
    u32 window_height;

    Viewport viewport;

    f32 unit_to_pixels;
    f32 pixels_to_units;

    f32 unit_to_pixels_2d;
    f32 pixels_to_units_2d;

    Matrix4x4 ortho_proj;

    // Gameplay
    Resources resources;
    Camera camera;
    EntityStorage entities;
};

global GameState game_state;
global InputState input_state;

global Program *current_shader;

#include "graphics.cpp"
#include "font.cpp"
#include "resources.cpp"
#include "hotload.cpp"
#include "camera.cpp"
#include "render.cpp"
#include "text_input.cpp"
#include "entity.cpp"
#include "console.cpp"
#include "game.cpp"
#include "input.cpp"
#include "commands.cpp"

LRESULT CALLBACK window_callback(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
    LRESULT result = 1;

    switch (message) {
        case WM_CREATE : {
            RECT window_rect;
            if (GetWindowRect(window, &window_rect)) {
                game_state.window_width  = window_rect.right - window_rect.left;
                game_state.window_height = window_rect.bottom - window_rect.top;
            }
        } break;

        case WM_CLOSE: {
            DestroyWindow(window);
        } break;

        case WM_SIZE: {
            game_state.window_width  = LOWORD(l_param);
            game_state.window_height = HIWORD(l_param);

            // Recalculate viewport to keep the aspect ratio
            {
                u32 w = game_state.window_width;
                u32 h = game_state.window_height;

                if (game_state.window_width * TARGET_ASPECT_H > game_state.window_height * TARGET_ASPECT_W) {
                    w = game_state.window_height * TARGET_ASPECT_W / TARGET_ASPECT_H;
                }
                else if (game_state.window_width * TARGET_ASPECT_H < game_state.window_height * TARGET_ASPECT_W) {
                    h = game_state.window_width * TARGET_ASPECT_H / TARGET_ASPECT_W;
                }

                game_state.viewport.left   = (game_state.window_width / 2) - w / 2;
                game_state.viewport.bottom = (game_state.window_height / 2) - h / 2;
                game_state.viewport.width  = w;
                game_state.viewport.height = h;

                glViewport(
                    game_state.viewport.left,
                    game_state.viewport.bottom,
                    game_state.viewport.width,
                    game_state.viewport.height
                );
            }

            game_state.unit_to_pixels = (f32)game_state.viewport.width / (f32)VIRTUAL_WINDOW_W;
            game_state.pixels_to_units = 1.0f / game_state.unit_to_pixels;

            game_state.unit_to_pixels_2d = (f32)game_state.viewport.width / (f32)VIRTUAL_WINDOW_W_2D;
            game_state.pixels_to_units_2d = 1.0f / game_state.unit_to_pixels_2d;
        } break;

        case WM_DESTROY: {
            if (opengl_context) {
                wglDeleteContext(opengl_context);
            }

            if (window_context) {
                ReleaseDC(window, window_context);
            }

            opengl_context = 0;
            window_context = 0;

            PostQuitMessage(0);
            exit(0);
        } break;

        case WM_KEYUP:
        case WM_KEYDOWN: {
            u16 virtual_code = LOWORD(w_param);

            u8 scan_code     = LOBYTE(HIWORD(l_param));
            bool alt_down    = (HIWORD(l_param) & KF_ALTDOWN) == KF_ALTDOWN;
            bool up_flag     = (HIWORD(l_param) & KF_UP) == KF_UP;

            if (up_flag) {
                handle_key_up(scan_code, virtual_code, alt_down);
            }
            else {
                handle_key_down(scan_code, virtual_code, alt_down);
            }
        } break;

        case WM_CHAR: {
            wchar_t pressed_char = (wchar_t)w_param;

            handle_char(pressed_char);
        } break;

        case WM_MOUSEMOVE: {
            input_state.mouse_x = GET_X_LPARAM(l_param);
            input_state.mouse_y = GET_Y_LPARAM(l_param);

            if (input_state.mouse_locked) {
                // Set cursor is in the screen coords not window coords!!
                // We need to convert.
                {
                    POINT pt;
                    pt.x = (i32)game_state.window_width  / 2;
                    pt.y = (i32)game_state.window_height / 2;
                    ClientToScreen(window, &pt);

                    SetCursorPos(pt.x, pt.y);
                }

                input_state.mouse_old_x = (i32)game_state.window_width  / 2;
                input_state.mouse_old_y = (i32)game_state.window_height / 2;
            }
        } break;

        default: {
            result = DefWindowProc(window, message, w_param, l_param);
        }
    }

    return result;
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR command_line, int show_code) {
    WNDCLASS window_class = {};

    window_class.lpfnWndProc   = window_callback;
    window_class.hInstance     = instance;
    window_class.lpszClassName = TEXT("WindowClass");
    //window_class.hIcon    // @Todo: add icon later

    RegisterClass(&window_class);

    RECT w_rect;
    w_rect.left   = 100;
    w_rect.right  = 100 + INITIAL_WINDOW_W;
    w_rect.bottom = 100 + INITIAL_WINDOW_H;
    w_rect.top    = 100;

    AdjustWindowRect(&w_rect, WS_OVERLAPPEDWINDOW, false);

    HWND window = CreateWindowEx(
        0,
        window_class.lpszClassName,
        TEXT("Window"),
        WS_OVERLAPPEDWINDOW,
        w_rect.left, w_rect.top,
        w_rect.right - w_rect.left, w_rect.bottom - w_rect.top,
        NULL, NULL,
        instance,
        NULL
    );

    if (window == NULL) {
        return 0;
    }

    init_gl_extensions();

    window_context = GetDC(window);
    opengl_context = create_gl_context(window_context);

    ShowWindow(window, show_code);
    UpdateWindow(window);

    init_game();
    init_renderer();
    init_commands();
    init_console();
    init_hotload();

    std::thread hotload_thread(hotload_watcher);

    MSG message = {};

    u64 current_time = millis();

    while (1) {
        while (PeekMessage(&message, NULL, 0, 0, PM_NOREMOVE)) {
            if (GetMessage(&message, NULL, 0, 0)) {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
            else {
                return 1;
            }
        }

#if !defined(RELEASE_MODE)
        process_hotload_queue(game_state.resources);
#endif

        handle_mouse_input();

        u64 new_time = millis();
        u64 frame_time = new_time - current_time;
        current_time = new_time;

        f32 delta_time = (f32)frame_time / 1000.0f;

        tick(delta_time);

        glClearColor(1.0, 0.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw_console(delta_time);

        render();

        glFlush();

        SwapBuffers(window_context);
    }

    hotload_thread.join();

    return 0;
}
