#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <gl/gl.h>
#include <stdio.h>

#include "platform.h"

HDC window_dc;
HGLRC opengl_rc;

internal void init_openGL(HWND window) {
    window_dc = GetDC(window);
        
    PIXELFORMATDESCRIPTOR pfd;

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.dwLayerMask = PFD_MAIN_PLANE;
    pfd.iPixelType = PFD_TYPE_COLORINDEX;
    pfd.cColorBits = 8;
    pfd.cDepthBits = 16;
    pfd.cAccumBits = 0;
    pfd.cStencilBits = 0;

    int pixelformat;
    
    if ((pixelformat = ChoosePixelFormat(window_dc, &pfd)) == 0) {
        printf("Choose pixelformat failed!");
    }

    if (SetPixelFormat(window_dc, pixelformat, &pfd) == 0) {
        printf("Set pixelformat failed!");
    }

    opengl_rc = wglCreateContext(window_dc);

    wglMakeCurrent(window_dc, opengl_rc);
}

LRESULT CALLBACK window_callback(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
    LRESULT result = 1;
    PAINTSTRUCT ps;

    switch (message) {
        case WM_CREATE : {
            init_openGL(window);
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

const wchar_t CLASS_NAME[]  = L"Sample Window Class";
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR command_line, int show_code) {

    WNDCLASS window_class = {};

    window_class.lpfnWndProc   = window_callback;
    window_class.hInstance     = instance;
    window_class.lpszClassName = CLASS_NAME;

    //window_class.style;   // @Todo: figure this out
    //window_class.hIcon    // @Todo: add icon later

    RegisterClass(&window_class);

    HWND window = CreateWindowEx(
        0,
        CLASS_NAME,
        L"ASD",
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

        glClearColor(1.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glFlush();

        SwapBuffers(window_dc);
    }


    return 0;
}