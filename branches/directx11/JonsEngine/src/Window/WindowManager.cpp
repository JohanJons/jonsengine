#include "include/Window/WindowManager.h"

#include <Windows.h>


namespace JonsEngine
{
    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
            case WM_PAINT:
            {
                PAINTSTRUCT ps;

                BeginPaint(hWnd, &ps);
                EndPaint(hWnd, &ps);

                break;
            }

            case WM_INPUT:
            {
                PUINT rawInputSize;
                RAWINPUT rawInput;

                GetRawInputData((HRAWINPUT) (lParam), RID_INPUT, &rawInput, rawInputSize, sizeof(RAWINPUTHEADER));

                if (rawInput.header.dwType == RIM_TYPEKEYBOARD)
                {

                }
                else if (rawInput.header.dwType == RIM_TYPEMOUSE)
                {

                }

                break;
            }

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
        }

        return 0;
    }


    WindowManager::WindowManager(const EngineSettings& engineSettings) : mInstanceHandle(GetModuleHandle(NULL))
    {
        // Register class
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = (HINSTANCE)mInstanceHandle;
        wcex.hIcon = nullptr;
        wcex.hCursor = nullptr;
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = mWindowTitle.c_str();
        wcex.hIconSm = nullptr;
        if (!RegisterClassEx(&wcex))
        {
          //  throw new Exception();
          //  ....
        }

        // Create window
        RECT rc = { 0, 0, 640, 480 };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
        g_hWnd = CreateWindow(L"TutorialWindowClass", L"Direct3D 11 Tutorial 7", WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
            nullptr);
     //   if (!g_hWnd)

        ShowWindow(g_hWnd, nCmdShow);

        // initialize RAW input
        RAWINPUTDEVICE rawInputDevices[2] =
        {
            { 0x01, 0x02, 0, hWnd },  // mouse
            { 0x01, 0x06, 0, hWnd }   // keyboard
        };
        if (!RegisterRawInputDevices(rawInputDevices, 2, sizeof(rawInputDevices[0]))
        {
            throw new Exception();
            ....
        }

        gWindowManagerInstance = this;
    }

    WindowManager::~WindowManager()
    {

    }


    void WindowManager::Poll()
    {

    }


    void WindowManager::SetMouseButtonCallback()
    {

    }

    void WindowManager::SetMouseButtonCallback(const MouseButtonCallback& callback)
    {

    }

    void WindowManager::SetMouseMotionCallback()
    {

    }

    void WindowManager::SetMouseMotionCallback(const MouseMotionCallback& callback)
    {

    }

    void WindowManager::SetMousePositionCallback()
    {

    }

    void WindowManager::SetMousePositionCallback(const MousePositionCallback& callback)
    {

    }

    void WindowManager::SetKeyCallback()
    {

    }

    void WindowManager::SetKeyCallback(const KeyCallback& callback)
    {

    }


    void WindowManager::ShowMouseCursor(bool show)
    {

    }

    void WindowManager::SetMousePosition(uint32_t x, uint32_t y)
    {

    }


    void WindowManager::SetFrameLimit(const uint64_t newFrameLimit)
    {

    }

    uint64_t WindowManager::GetFrameLimit() const
    {
        // TODO
        return 0;
    }

    uint64_t WindowManager::GetCurrentFPS() const
    {
        // TODO
        return 0;
    }


    void WindowManager::SetFullscreen(bool fullscreen)
    {

    }

    void WindowManager::SetScreenResolution(const uint32_t width, const uint32_t height)
    {

    }

    void WindowManager::SetWindowTitle(const std::string& windowTitle)
    {

    }

    void WindowManager::SetFOV(const float FOV)
    {

    }


    bool WindowManager::GetFullscreen() const
    {
        return mFullscreen;
    }

    uint32_t WindowManager::GetScreenWidth() const
    {
        return mScreenWidth;
    }

    uint32_t WindowManager::GetScreenHeight() const
    {
        return mScreenHeight;
    }

    const std::string& WindowManager::GetWindowTitle() const
    {
        return mWindowTitle;
    }

    float WindowManager::GetFOV() const
    {
        return mFOV;
    }
}