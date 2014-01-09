
/*
4kGL-Example.
A simple demo example to test 4kGL.
*/


#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <mmsystem.h>
#include <GL/gl.h>

#include "4kGL.h"


#if defined(_MSC_VER)
    extern "C" int _fltused = 0;
#endif


// Message handler:

LRESULT CALLBACK
WndProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}


// Initialization and shutdown:

bool
Initialize () {

    // no WinMain so get the module handle:
    g_hInstance = GetModuleHandle(NULL);
    if (g_hInstance == NULL)
        return false;

    // register the window class:
    WNDCLASS wc;

    wc.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = g_hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = WINDOW_TITLE;

    if (RegisterClass(&wc) == 0)
        return false;

    // create the window:
    g_hWnd = CreateWindow(
        WINDOW_TITLE,                                             // class name
        WINDOW_TITLE,                                             // title
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // style
        CW_USEDEFAULT, CW_USEDEFAULT,                             // position
        CW_USEDEFAULT, CW_USEDEFAULT,                             // size
        NULL,                                                     // no parent
        NULL,                                                     // no menu
        g_hInstance,                                              // instance
        NULL                                                      // no special
    );

    if (g_hWnd == NULL)
        return false;

    // setup OpenGL:
    g_hDC = GetDC(g_hWnd);
    if (g_hDC == NULL)
        return false;

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,                             // version number
        PFD_DRAW_TO_WINDOW |           // support window
        PFD_SUPPORT_OPENGL |           // support OpenGL
        PFD_DOUBLEBUFFER,              // double buffered
        PFD_TYPE_RGBA,                 // RGBA type
        32,                            // 32-bit color depth
        0, 0, 0, 0, 0, 0,              // color bits ignored
        0,                             // no alpha buffer
        0,                             // shift bit ignored
        0,                             // no accumulation buffer
        0, 0, 0, 0,                    // accumulation bits ignored
        32,                            // 32-bit z buffer
        0,                             // no stencil buffer
        0,                             // no auxiliary buffer
        PFD_MAIN_PLANE,                // main layer
        0,                             // reserved
        0, 0, 0,                       // layer masks ignored
    };

    int pixelFormat = ChoosePixelFormat(g_hDC, &pfd);
    if (pixelFormat == 0)
        return false;

    if (SetPixelFormat(g_hDC, pixelFormat, &pfd) == FALSE)
        return false;

    g_hGLRC = wglCreateContext(g_hDC);
    if (g_hGLRC == NULL)
        return false;

    if (wglMakeCurrent(g_hDC, g_hGLRC) == FALSE)
        return false;

    return true;
}


void
Shutdown (UINT uExitCode) {

    // release OpenGL context:
    if (g_hGLRC != NULL) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(g_hGLRC);
    }

    if (g_hDC != NULL)
        ReleaseDC(g_hWnd, g_hDC);

    // destroy the window and unregister the class:
    if (g_hWnd != NULL)
        DestroyWindow(g_hWnd);

    WNDCLASS wc;
    if (GetClassInfo(g_hInstance, WINDOW_TITLE, &wc) != 0)
        UnregisterClass(WINDOW_TITLE, g_hInstance);

    // without WinMainCRTStartup() we must exit the process ourselves:
    ExitProcess(uExitCode);
}


// Cube vertex, quad and color data:

GLfloat cube_vertex[8][3] = {
    {  1.0f, -1.0f,  1.0f },
    {  1.0f,  1.0f,  1.0f },
    {  1.0f, -1.0f, -1.0f },
    {  1.0f,  1.0f, -1.0f },
    { -1.0f, -1.0f, -1.0f },
    { -1.0f,  1.0f, -1.0f },
    { -1.0f, -1.0f,  1.0f },
    { -1.0f,  1.0f,  1.0f }
};

GLint cube_quad[6][4] = {
    { 1, 0, 2, 3 },
    { 3, 2, 4, 5 },
    { 5, 4, 6, 7 },
    { 7, 6, 0, 1 },
    { 5, 7, 1, 3 },
    { 6, 4, 2, 0 }
};

GLfloat cube_color[3][6] = {
    { 0.5f,  0.6f, 0.7f, 0.8f, 0.9f, 1.0f },
    { 0.3f,  0.3f, 0.3f, 0.3f, 0.3f, 0.3f },
    { 0.3f, 0.75f, 0.9f, 1.0f, 1.0f, 1.0f }
};


// A simple rotating cubes effect:

GLfloat rotation = 0.0f;
GLfloat alpha = 0.5f;
bool alpha_increasing = true;

void
Update () {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.092f, 0.184f, 0.229f, 0.0f);

    // draw two cubes:
    for (int i = 0; i < 2; i++) {
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -4.0f);

        // outer cube, bigger:
        if (i == 0) {
            glRotatef(rotation, 0.0f, 1.0f, 1.0f);
            glRotatef(rotation, 1.0f, 0.0f, 1.0f);
            glScalef(3.5f, 3.5f, 3.5f);
        }

        // inner cube, no scaling, different rotation:
        else {
            glRotatef(rotation, 1.0f, 0.0f, 0.0f);
            glRotatef(rotation, 0.0f, 1.0f, 0.0f);
        }

        // draw the current cube, change color for each quad:
        glBegin(GL_QUADS);
        for (int quad = 0; quad < 6; quad++) {
            glColor4f(cube_color[0][quad],
                      cube_color[1][quad],
                      cube_color[2][quad],
                      alpha);
            for (int vertex = 0; vertex < 4; vertex++)
                glVertex3fv(&cube_vertex[cube_quad[quad][vertex]][0]);
        }
        glEnd();
    }

    // change alpha:
    if (alpha <= 0.2f)
        alpha_increasing = true;
    else if (alpha >= 0.9f)
        alpha_increasing = false;

    alpha += alpha_increasing ? 0.005f : -0.005f;

    // change rotation:
    rotation += 0.5f;
    if (rotation >= 360.0f)
        rotation = 0.0f;
}


void
Loop () {
    MSG msg;
    bool done = false;

    // this game loop is far from ideal since rendering/logic are tied
    // and timeGetTime() and Sleep() are not precise, but it is
    // small and works well enough for simple 4k games/demos

    // for a better game loop see:
    // <http://gafferongames.com/game-physics/fix-your-timestep>

    while (!done) {
        DWORD dwStart = timeGetTime();

        // poll windows events:
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0) {
            if (msg.message == WM_QUIT)
                done = true;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // update and swap buffers:
        Update();
        SwapBuffers(g_hDC);

        // sleep until next step:
        DWORD dwDelta = timeGetTime() - dwStart;
        if (dwDelta < STEP_RATE) {
            Sleep(STEP_RATE - dwDelta);
        }
    }
}


// A helper to resize the window with respect to the client area:

void
ResizeClientWindow (HWND hWnd, UINT uWidth, UINT uHeight) {
    RECT rcClient, rcWindow;

    GetClientRect(hWnd, &rcClient);
    GetWindowRect(hWnd, &rcWindow);

    MoveWindow(hWnd,
               rcWindow.left,
               rcWindow.top,
               uWidth + (rcWindow.right - rcWindow.left) - rcClient.right,
               uHeight + (rcWindow.bottom - rcWindow.top) - rcClient.bottom,
               FALSE);
}


// Entry point:

void
EntryPoint () {
    if (!Initialize()) {
        MessageBox(NULL, "Initialization failed.", "Error", MB_OK | MB_ICONERROR);
        Shutdown(1);
    }

    ResizeClientWindow(g_hWnd, WINDOW_WIDTH, WINDOW_HEIGHT);

    // setup OpenGL features:
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // setup OpenGL projection:
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);

    // precomputed for 60.0 FOV and 1.0 zNear: tan(60/360 * pi)
    // to avoid using gluPerspective:
    GLdouble fH = 0.5773502691896257;
    GLdouble fW = fH * ((double) WINDOW_WIDTH / (double) WINDOW_HEIGHT);
    glFrustum(-fW, fW, -fH, fH, 1.0f, 100.0f);

    glMatrixMode(GL_MODELVIEW);

    ShowWindow(g_hWnd, SW_SHOW);
    Loop();
    Shutdown(0);
}

