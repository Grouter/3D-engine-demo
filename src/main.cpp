// Uncomment for release mode
//#define NDEBUG

// @Todo: check if RVO is happening

#include <windows.h>
#include <glew.h>
#include <gl/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <assert.h>
#include <algorithm>    // @Todo: implement own sort?

#include "platform.h"

global HDC   window_dc;
global HGLRC opengl_rc;

#include "array.h"
#include "array.cpp"
#include "bucket_array.h"
#include "bucket_array.cpp"
#include "math/math.h"
#include "math/vector.h"
#include "math/vector.cpp"
#include "math/matrix.h"
#include "math/matrix.cpp"
#include "math/quaternion.h"
#include "math/quaternion.cpp"
#include "camera.h"
#include "camera.cpp"
#include "graphics.h"
#include "graphics.cpp"
#include "resources.h"
#include "resources.cpp"
#include "entity.h"
#include "entity.cpp"

struct GameState {
    u32 window_width;
    u32 window_height;
    Resources resources;
    Camera camera;

    EntityStorage entities;
};

global GameState game_state;

#include "render.cpp"
#include "input.cpp"
#include "game.h"
#include "game.cpp"

internal void init_opengl(HWND window) {
    window_dc = GetDC(window);

    PIXELFORMATDESCRIPTOR desiredPixelFormat;
    desiredPixelFormat.nSize    = sizeof(PIXELFORMATDESCRIPTOR);
    desiredPixelFormat.nVersion = 1;

    desiredPixelFormat.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    desiredPixelFormat.dwLayerMask  = PFD_MAIN_PLANE;
    desiredPixelFormat.iPixelType   = PFD_TYPE_COLORINDEX;
    desiredPixelFormat.cColorBits   = 32;
    desiredPixelFormat.cAlphaBits   = 8;
    desiredPixelFormat.cDepthBits   = 16;

    // Find and set pixelformat
    {
        int suggestedPixelformatIndex = ChoosePixelFormat(window_dc, &desiredPixelFormat);

        PIXELFORMATDESCRIPTOR suggestedPixelFormat;
        DescribePixelFormat(window_dc, suggestedPixelformatIndex, sizeof(suggestedPixelFormat), &suggestedPixelFormat);

        SetPixelFormat(window_dc, suggestedPixelformatIndex, &suggestedPixelFormat);
    }

    opengl_rc = wglCreateContext(window_dc);

    wglMakeCurrent(window_dc, opengl_rc);

    OutputDebugStringA("OpenGL initialized:\n");

    OutputDebugStringA("    Vendor: ");
    OutputDebugStringA((char*)glGetString(GL_VENDOR));
    OutputDebugStringA("\n");

    OutputDebugStringA("    Renderer: ");
    OutputDebugStringA((char*)glGetString(GL_RENDERER));
    OutputDebugStringA("\n");

    OutputDebugStringA("    Version: ");
    OutputDebugStringA((char*)glGetString(GL_VERSION));
    OutputDebugStringA("\n");
}

LRESULT CALLBACK window_callback(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
    LRESULT result = 1;

    switch (message) {
        case WM_CREATE : {
            init_opengl(window);

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

            update_perspective(game_state.camera, game_state.window_width, game_state.window_height);
        } break;

        case WM_DESTROY: {
            if (opengl_rc) {
                wglDeleteContext(opengl_rc);
            }

            if (window_dc) {
                ReleaseDC(window, window_dc);
            }

            opengl_rc = 0;
            window_dc = 0;

            PostQuitMessage(0);
        } break;

        case WM_KEYUP:
        case WM_KEYDOWN: {
            u16 virtual_code = LOWORD(w_param);

            u8 scan_code     = LOBYTE(HIWORD(l_param));
            bool alt_down    = (HIWORD(l_param) & KF_ALTDOWN) == KF_ALTDOWN;
            bool up_flag     = (HIWORD(l_param) & KF_UP) == KF_UP;
            bool repeat_flag = (HIWORD(l_param) & KF_REPEAT) == KF_REPEAT;

            if (repeat_flag) break; // We will handle repeat our way...

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
    window_class.lpszClassName = TEXT("CastleDemoWindowClass");
    //window_class.hIcon    // @Todo: add icon later

    RegisterClass(&window_class);

    HWND window = CreateWindowEx(
        0,
        window_class.lpszClassName,
        TEXT("Castle Demo"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        instance,
        NULL
    );

    if (window == NULL) {
        return 0;
    }

    ShowWindow(window, show_code);
    UpdateWindow(window);

    GLenum glew_state = glewInit();
    if (glew_state != GLEW_OK) {
        printf("Error while initializing GL extensions!\n");
        exit(1);
    }

    init();

    MSG message = {};

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

        tick();

        glClearColor(1.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        render();

        glFlush();

        SwapBuffers(window_dc);
    }

    return 0;
}
