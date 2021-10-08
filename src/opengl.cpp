// To initialzie GL extensions, we need to create a fake window
// which will be destroyed after extensions are initialzied...
internal void init_gl_extensions() {
    WNDCLASS window_class = {};

    window_class.lpfnWndProc   = DefWindowProcA;
    window_class.hInstance     = GetModuleHandle(0);
    window_class.lpszClassName = TEXT("FakeWindowClass");

    RegisterClass(&window_class);

    HWND fake_window = CreateWindowEx(
        0,
        window_class.lpszClassName,
        TEXT("FakeWindow"),
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        0, 0, 1, 1,
        NULL, NULL,
        window_class.hInstance,
        NULL
    );

    HDC fake_DC = GetDC(fake_window);
    if (!fake_DC) {
        printf("Create a fake window!\n");
        exit(1);
    }

    PIXELFORMATDESCRIPTOR fake_PFD = {};
    fake_PFD.nSize       = sizeof(PIXELFORMATDESCRIPTOR);
    fake_PFD.nVersion    = 1;
    fake_PFD.dwFlags     = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    fake_PFD.dwLayerMask = PFD_MAIN_PLANE;
    fake_PFD.cColorBits  = 32;
    fake_PFD.cAlphaBits  = 8;
    fake_PFD.cDepthBits  = 16;

    int fake_PFD_ID = ChoosePixelFormat(fake_DC, &fake_PFD);
    if (!fake_PFD_ID) {
        printf("Could not choos a fake pixel format!\n");
        exit(1);
    }

    if (!SetPixelFormat(fake_DC, fake_PFD_ID, &fake_PFD)) {
        printf("Could not set a fake pixel format!\n");
        exit(1);
    }

    HGLRC fake_RC = wglCreateContext(fake_DC);
    if (!fake_RC) {
        printf("Could create a fake gl render context!\n");
        exit(1);
    }

    if (!wglMakeCurrent(fake_DC, fake_RC)) {
        printf("Could not set a fake render context!\n");
        exit(1);
    }

    GLenum glew_state = glewInit();
    if (glew_state != GLEW_OK) {
        printf("Error while initializing GL extensions!\n");
        exit(1);
    }

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(fake_RC);
    ReleaseDC(fake_window, fake_DC);
    DestroyWindow(fake_window);
}

internal HGLRC create_gl_context(HDC window_context) {
    const i32 pixel_attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
        WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
        WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB,     32,
        WGL_ALPHA_BITS_ARB,     8,
        WGL_DEPTH_BITS_ARB,     24,
        WGL_STENCIL_BITS_ARB,   8,
        WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
        WGL_SAMPLES_ARB,        4,
        0
    };

    i32 pixel_format_ID;
    u32 num_formats;

    bool status = wglChoosePixelFormatARB(window_context, pixel_attribs, 0, 1, &pixel_format_ID, &num_formats);
    if (!status || num_formats == 0) {
        printf("GL could not choose a pixel format!\n");
        exit(1);
    }

    PIXELFORMATDESCRIPTOR pfd;
    DescribePixelFormat(window_context, pixel_format_ID, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
    SetPixelFormat(window_context, pixel_format_ID, &pfd);

    i32 context_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    HGLRC gl_render_context = wglCreateContextAttribsARB(window_context, NULL, context_attribs);
    if (gl_render_context == NULL) {
        printf("OpenGL render context could not be created!\n");
        exit(1);
    }

    if (!wglMakeCurrent(window_context, gl_render_context)) {
        printf("Could not set opengl render context to window context\n");
        exit(1);
    }

    // V-Sync
    wglSwapIntervalEXT(1);

    // Culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Depth tests
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

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

    return gl_render_context;
}