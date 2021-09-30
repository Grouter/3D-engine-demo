#ifndef UNICODE
#define UNICODE
#endif

// Uncomment for release mode
//#define NDEBUG

#include <windows.h>
#include <glew.h>
#include <gl/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "platform.h"

global HDC   window_dc;
global HGLRC opengl_rc;

#include "array.h"
#include "array.cpp"
#include "math/vector.h"
#include "math/vector.cpp"
#include "math/matrix.h"
#include "math/matrix.cpp"
#include "graphics.h"
#include "graphics.cpp"
#include "resources.h"
#include "resources.cpp"

global Resources resources;

#include "entity.h"
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
}

LRESULT CALLBACK window_callback(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
    LRESULT result = 1;

    switch (message) {
        case WM_CREATE : {
            init_opengl(window);
        } break;

        case WM_CLOSE: {
            DestroyWindow(window);
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

        default: {
            result = DefWindowProc(window, message, w_param, l_param);
        }
    }

    return result;
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR command_line, int show_code) {
    const wchar_t CLASS_NAME[]  = L"GameWindowClass";

    WNDCLASS window_class = {};

    window_class.lpfnWndProc   = window_callback;
    window_class.hInstance     = instance;
    window_class.lpszClassName = CLASS_NAME;
    //window_class.hIcon    // @Todo: add icon later

    RegisterClass(&window_class);

    HWND window = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Game",
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
        //printf("Error while initializing GL extensions!\n");
        //exit(1);
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
