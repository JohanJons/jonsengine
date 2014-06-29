#include "include/Window/WindowManagerImpl.h"

#include "include/Core/EngineSettings.h"
#include "include/Core/Logging/Logger.h"

#include <exception>

namespace JonsEngine
{
    static WindowManagerImpl* gWindowManagerImpl = nullptr;


    LRESULT CALLBACK WindowManagerImpl::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
                PUINT rawInputSize = nullptr;
                RAWINPUT rawInput;

                GetRawInputData((HRAWINPUT) (lParam), RID_INPUT, &rawInput, rawInputSize, sizeof(RAWINPUTHEADER));

                if (rawInput.header.dwType == RIM_TYPEKEYBOARD)
                    gWindowManagerImpl->ProcessKeyboardInput(rawInput.data.keyboard);
                else if (rawInput.header.dwType == RIM_TYPEMOUSE)
                    gWindowManagerImpl->ProcessMouseInput(rawInput.data.mouse);

                break;
            }

            case WM_DESTROY:
            {
                PostQuitMessage(0);
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
        mShowMouseCursor(false), mFullscreen(engineSettings.mFullscreen), mFOV(engineSettings.mFOV), mPreviousMouseX(0.0), mPreviousMouseY(0.0), mMouseButtonCallback(nullptr), mMouseMotionCallback(nullptr),
        mMousePositionCallback(nullptr), mKeyCallback(nullptr), mInstanceHandle(GetModuleHandle(NULL)), mWindowHandle(nullptr)
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
            JONS_LOG_ERROR(mLogger, "WindowManagerImpl::WindowManagerImpl(): RegisterClassEx failed: " + win32Error);
			throw std::runtime_error("WindowManagerImpl::WindowManagerImpl(): RegisterClassEx failed: " + win32Error);
        }

        // Create window
        RECT windowRect = {0, 0, mScreenWidth, mScreenHeight};
        AdjustWindowRect(&windowRect, WS_MAXIMIZE, FALSE);
        mWindowHandle = CreateWindow(wcex.lpszClassName, wcex.lpszClassName, WS_MAXIMIZE | WS_OVERLAPPED, CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
            nullptr, nullptr, mInstanceHandle, nullptr);
        if (!mWindowHandle)
        {
			const std::string win32Error = GetWin32ErrorString();
			JONS_LOG_ERROR(mLogger, "WindowManagerImpl::WindowManagerImpl(): CreateWindow failed: " + win32Error);
			throw std::runtime_error("WindowManagerImpl::WindowManagerImpl(): CreateWindow failed: " + win32Error);
        }

        ShowCursor(mShowMouseCursor);
        ShowWindow(mWindowHandle, SW_SHOWNORMAL);

        // initialize RAW input
        RAWINPUTDEVICE rawInputDevices[2] =
        {
            { 0x01, 0x02, 0, mWindowHandle },  // mouse
            { 0x01, 0x06, 0, mWindowHandle }   // keyboard
        };

        if (!RegisterRawInputDevices(rawInputDevices, 2, sizeof(rawInputDevices[0])))
        {
            const std::string win32Error = GetWin32ErrorString();
            JONS_LOG_ERROR(mLogger, "WindowManagerImpl::WindowManagerImpl(): RegisterRawInputDevices failed: " + win32Error);
            throw std::runtime_error("WindowManagerImpl::WindowManagerImpl(): RegisterRawInputDevices failed: " + win32Error);
        }

        gWindowManagerImpl = this;
    }

    WindowManagerImpl::~WindowManagerImpl()
    {
        DestroyWindow(mWindowHandle);
    }


    void WindowManagerImpl::Poll()
    {
        MSG msg = {0};
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Dispatch waiting events for each event type to registered callbacks
        if (mMouseButtonCallback)
        {
            for (MouseButtonEvent& ev : mMouseButtonEvents)
                mMouseButtonCallback(ev);
        }

        if (mMouseMotionCallback)
        {
            for (MouseMotionEvent& ev : mMouseMotionEvents)
                mMouseMotionCallback(ev);
        }

        if (mKeyCallback)
        {
            for (KeyEvent& ev : mKeyEvents)
                mKeyCallback(ev);
        }

        if (mMousePositionCallback)
        {
            POINT cursorPos;

            if (GetCursorPos(&cursorPos));
            {
                ScreenToClient(mWindowHandle, &cursorPos);

                mMousePositionCallback(MousePositionEvent(mPreviousMouseX, mPreviousMouseY, cursorPos.x, cursorPos.y));

                mPreviousMouseX = cursorPos.x;
                mPreviousMouseY = cursorPos.y;
            }
        }

        mMouseButtonEvents.clear();
        mMouseMotionEvents.clear();
        mKeyEvents.clear();

        UpdateWindow(mWindowHandle);    // needed?
    }

    void WindowManagerImpl::SetMouseButtonCallback()
    {
        mMouseButtonCallback = nullptr;
    }

    void WindowManagerImpl::SetMouseButtonCallback(const MouseButtonCallback& callback)
    {
        mMouseButtonCallback = callback;
    }

    void WindowManagerImpl::SetMouseMotionCallback()
    { 
        mMouseMotionCallback = nullptr;
    }

    void WindowManagerImpl::SetMouseMotionCallback(const MouseMotionCallback& callback)
    {
        mMouseMotionCallback = callback;
    }

    void WindowManagerImpl::SetMousePositionCallback()
    {
        mMousePositionCallback = nullptr;
    }

    void WindowManagerImpl::SetMousePositionCallback(const MousePositionCallback& callback)
    {
        mMousePositionCallback = callback;
    }

    void WindowManagerImpl::SetKeyCallback()
    {
        mKeyCallback = nullptr;
    }
    
    void WindowManagerImpl::SetKeyCallback(const KeyCallback& callback)
    {
        mKeyCallback = callback;
    }

    void WindowManagerImpl::ShowMouseCursor(const bool show)
    {
        mShowMouseCursor = show;
        ShowCursor(mShowMouseCursor);
    }

	void WindowManagerImpl::SetMousePosition(const uint32_t x, const uint32_t y)
    {
        POINT cursorPos = {x, y};

        ClientToScreen(mWindowHandle, &cursorPos);
        SetCursorPos(cursorPos.x, cursorPos.y);
    }


	void WindowManagerImpl::SetFullscreen(const bool fullscreen)
    {
        mFullscreen = fullscreen;
    }

    void WindowManagerImpl::SetScreenResolution(const uint32_t width, const uint32_t height)
    {
        mScreenWidth = width;
        mScreenHeight = height;
    }

    void WindowManagerImpl::SetWindowTitle(const std::string& windowTitle)
    {
        mWindowTitle = windowTitle;
    }

    void WindowManagerImpl::SetFOV(const float FOV)
    {
        mFOV = FOV;
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

    bool WindowManagerImpl::GetShowMouseCursor() const
    {
        return mShowMouseCursor;
    }

    float WindowManagerImpl::GetFOV() const
    {
        return mFOV;
    }


    void WindowManagerImpl::ProcessKeyboardInput(const RAWKEYBOARD& keyInput)
    {
        if (keyInput.Message != WM_KEYDOWN && keyInput.Message != WM_SYSKEYDOWN)
            return;


    }

    void WindowManagerImpl::ProcessMouseInput(const RAWMOUSE& mouseInput)
    {

    }
}