#include "include/Window/WindowManagerImpl.h"

#include "include/Core/EngineSettings.h"
#include "include/Core/Logging/Logger.h"

#include <exception>

namespace JonsEngine
{
    static WindowManagerImpl* gWindowManagerImpl = nullptr;


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

	std::string GetWin32ErrorString()
	{
		DWORD dwLastError = GetLastError();
		TCHAR lpBuffer[256] = "?";

		if (dwLastError != 0)
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwLastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), lpBuffer, 255, NULL);

		return std::string(lpBuffer);
	}


    WindowManagerImpl::WindowManagerImpl(const EngineSettings& engineSettings, Logger& logger) : mLogger(logger), mWindowTitle(engineSettings.mWindowTitle), mScreenWidth(engineSettings.mWindowWidth), mScreenHeight(engineSettings.mWindowHeight),
        mShowMouseCursor(false), mFullscreen(engineSettings.mFullscreen), mFOV(engineSettings.mFOV), mPreviousMouseX(0.0), mPreviousMouseY(0.0), mInstanceHandle(GetModuleHandle(NULL)), mWindowHandle(nullptr)
    {
        // Register class
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = mInstanceHandle;
		wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wcex.hCursor = nullptr;
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = mWindowTitle.c_str();
		wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;
        if (!RegisterClassEx(&wcex))
        {
			const std::string win32Error = GetWin32ErrorString();
            JONS_LOG_ERROR(mLogger, "WindowManagerImpl::WindowManagerImpl(): RegisterClassEx failed" + win32Error);
			throw std::runtime_error("WindowManagerImpl::WindowManagerImpl(): RegisterClassEx failed" + win32Error);
        }

        // Create window
        RECT rc = {0, 0, mScreenWidth, mScreenHeight};
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
        mWindowHandle = CreateWindow(wcex.lpszClassName, wcex.lpszClassName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, mInstanceHandle, nullptr);
        if (!mWindowHandle)
        {
			const std::string win32Error = GetWin32ErrorString();
			JONS_LOG_ERROR(mLogger, "WindowManagerImpl::WindowManagerImpl(): CreateWindow failed" + win32Error);
			throw std::runtime_error("WindowManagerImpl::WindowManagerImpl(): CreateWindow failed" + win32Error);
        }

        ShowWindow(mWindowHandle, SW_SHOWMAXIMIZED);

        // initialize RAW input
        RAWINPUTDEVICE rawInputDevices[2] =
        {
            { 0x01, 0x02, 0, mWindowHandle },  // mouse
            { 0x01, 0x06, 0, mWindowHandle }   // keyboard
        };

        if (!RegisterRawInputDevices(rawInputDevices, 2, sizeof(rawInputDevices[0])))
        {
            //throw new Exception();
            // ....
        }

        gWindowManagerImpl = this;
    }

    WindowManagerImpl::~WindowManagerImpl()
    {

    }


    void WindowManagerImpl::Poll()
    { 

    }

    void WindowManagerImpl::SetMouseButtonCallback()
    {

    }

    void WindowManagerImpl::SetMouseButtonCallback(const MouseButtonCallback& callback)
    {

    }

    void WindowManagerImpl::SetMouseMotionCallback()
    { 

    }

    void WindowManagerImpl::SetMouseMotionCallback(const MouseMotionCallback& callback)
    {
    }

    void WindowManagerImpl::SetMousePositionCallback()
    { 
    }

    void WindowManagerImpl::SetMousePositionCallback(const MousePositionCallback& callback)
    {
    }

    void WindowManagerImpl::SetKeyCallback()
    {

    }
    
    void WindowManagerImpl::SetKeyCallback(const KeyCallback& callback)
    {
    }

    void WindowManagerImpl::ShowMouseCursor(const bool show)
    {

    }

	void WindowManagerImpl::SetMousePosition(const uint32_t x, const uint32_t y)
    {

    }


	void WindowManagerImpl::SetFullscreen(const bool fullscreen)
    {

    }

    void WindowManagerImpl::SetScreenResolution(const uint32_t width, const uint32_t height)
    {

    }

    void WindowManagerImpl::SetWindowTitle(const std::string& windowTitle)
    {

    }

    void WindowManagerImpl::SetFOV(const float FOV)
    {

    }

    bool WindowManagerImpl::GetFullscreen() const
    {
        return mFullscreen;
    }

    uint32_t WindowManagerImpl::GetScreenWidth() const
    {
        return mScreenWidth;
    }

    uint32_t WindowManagerImpl::GetScreenHeight() const
    {
        return mScreenHeight;
    }

    const std::string& WindowManagerImpl::GetWindowTitle() const
    {
        return mWindowTitle;
    }

    float WindowManagerImpl::GetFOV() const
    {
        return mFOV;
    }
}