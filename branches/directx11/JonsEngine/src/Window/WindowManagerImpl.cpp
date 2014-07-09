#include "include/Window/WindowManagerImpl.h"

#include "include/Core/EngineSettings.h"
#include "include/Core/Logging/Logger.h"
#include "include/Core/Utils/Win32.h"

#include <exception>


namespace JonsEngine
{
    static WindowManagerImpl* gWindowManagerImpl = nullptr;

    const LONG gWindowedStyle = WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME;


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
                RAWINPUT rawInput;
                UINT rawInputSize = sizeof(rawInput);

                GetRawInputData((HRAWINPUT)(lParam), RID_INPUT, &rawInput, &rawInputSize, sizeof(RAWINPUTHEADER));

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
		mShowMouseCursor(false), mFullscreen(engineSettings.mFullscreen), mFOV(engineSettings.mFOV), mCurrentMouseX(0), mCurrentMouseY(0), mPreviousMouseX(0),
		mPreviousMouseY(0), mMouseButtonCallback(nullptr), mMousePositionCallback(nullptr), mKeyCallback(nullptr), mInstanceHandle(GetModuleHandle(NULL)), mWindowHandle(nullptr)
    {
        // Register class
        WNDCLASSEX wcex;
        ZeroMemory(&wcex, sizeof(wcex));
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = mInstanceHandle;
        wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wcex.hCursor = nullptr;
        wcex.lpszMenuName = mWindowTitle.c_str();
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
        AdjustWindowRect(&windowRect, gWindowedStyle, FALSE);
        mWindowHandle = CreateWindow(wcex.lpszClassName, wcex.lpszClassName, gWindowedStyle, CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, mInstanceHandle,
            nullptr);
        if (!mWindowHandle)
        {
            const std::string win32Error = GetWin32ErrorString();
            JONS_LOG_ERROR(mLogger, "WindowManagerImpl::WindowManagerImpl(): CreateWindow failed: " + win32Error);
            throw std::runtime_error("WindowManagerImpl::WindowManagerImpl(): CreateWindow failed: " + win32Error);
        }

        ShowCursor(mShowMouseCursor);
        ShowWindow(mWindowHandle, SW_SHOWNORMAL);
        UpdateWindow(mWindowHandle);

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
        MSG msg = { 0 };
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Dispatch waiting events for each event type to registered callbacks
        if (mMousePositionCallback)
        {
			mMousePositionCallback(MousePositionEvent(mPreviousMouseX, mPreviousMouseY, mCurrentMouseX, mCurrentMouseY));

			mPreviousMouseX = mCurrentMouseX;
			mPreviousMouseY = mCurrentMouseY;
        }

        if (mMouseButtonCallback)
        {
            for (MouseButtonEvent& ev : mMouseButtonEvents)
                mMouseButtonCallback(ev);
        }

        if (mKeyCallback)
        {
            for (KeyEvent& ev : mKeyEvents)
                mKeyCallback(ev);
        }

        mMouseButtonEvents.clear();
        mKeyEvents.clear();
    }

    void WindowManagerImpl::SetMouseButtonCallback()
    {
        mMouseButtonCallback = nullptr;
    }

    void WindowManagerImpl::SetMouseButtonCallback(const MouseButtonCallback& callback)
    {
        mMouseButtonCallback = callback;
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
        if (show)
            while (ShowCursor(true) <= 0);
        else
            while (ShowCursor(false) >= 0);

        SendMessage(mWindowHandle, WM_SETCURSOR, (WPARAM)mWindowHandle, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));     // needed?

        UpdateWindow(mWindowHandle);

        mShowMouseCursor = show;
    }

    void WindowManagerImpl::SetMousePosition(const uint32_t x, const uint32_t y)
    {
        POINT cursorPos = { x, y };

        ClientToScreen(mWindowHandle, &cursorPos);
        SetCursorPos(cursorPos.x, cursorPos.y);

        UpdateWindow(mWindowHandle);
    }


    void WindowManagerImpl::SetFullscreen(const bool fullscreen)
    {
		if (mFullscreen == fullscreen)
			return;

	    SendMessage(mWindowHandle, WM_JONS_FULLSCREEN, mFullscreen, mScreenWidth | (mScreenHeight << 16)));

        mFullscreen = fullscreen;
    }

    void WindowManagerImpl::SetScreenResolution(const uint32_t width, const uint32_t height)
    {
        SendMessage(mWindowHandle, WM_JONS_RESIZE, mFullscreen, mScreenWidth | (mScreenHeight << 16));

        mScreenWidth = width;
        mScreenHeight = height;
    }

    void WindowManagerImpl::SetWindowTitle(const std::string& windowTitle)
    {
        mWindowTitle = windowTitle;

        SetWindowText(mWindowHandle, mWindowTitle.c_str());

        UpdateWindow(mWindowHandle);
    }

    void WindowManagerImpl::SetFOV(const float FOV)
    {
        mFOV = FOV;
    }

    bool WindowManagerImpl::GetFullscreen() const
    {
        return mFullscreen;
    }

    uint16_t WindowManagerImpl::GetScreenWidth() const
    {
        return mScreenWidth;
    }

    uint16_t WindowManagerImpl::GetScreenHeight() const
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
        if (!mKeyCallback)
            return;

        SHORT altDown = GetKeyState(VK_MENU);
        SHORT shiftDown = GetKeyState(VK_SHIFT);
        SHORT ctrlDown = GetKeyState(VK_CONTROL);

        mKeyEvents.push_back(KeyEvent(
            [&]()
        {
            switch (keyInput.VKey)
            {
				case VK_SPACE:          return SPACE;
				case VK_MULTIPLY:       return ASTERISK;
				case VK_ADD:            return PLUS;
				case VK_SEPARATOR:      return COMMA;
				case VK_SUBTRACT:       return MINUS;
				case VK_DECIMAL:        return DOT;
				case 0x30:              return ZERO;
				case 0x31:              return ONE;
				case 0x32:              return TWO;
				case 0x33:              return THREE;
				case 0x34:              return FOUR;
				case 0x35:              return FIVE;
				case 0x36:              return SIX;
				case 0x37:              return SEVEN;
				case 0x38:              return EIGHT;
				case 0x39:              return NINE;
				case 0x41:              return A;
				case 0x42:              return B;
				case 0x43:              return C;
				case 0x44:              return D;
				case 0x45:              return E;
				case 0x46:              return F;
				case 0x47:              return G;
				case 0x48:              return H;
				case 0x49:              return I;
				case 0x4A:              return J;
				case 0x4B:              return K;
				case 0x4C:              return L;
				case 0x4D:              return M;
				case 0x4E:              return N;
				case 0x4F:              return O;
				case 0x50:              return P;
				case 0x51:              return Q;
				case 0x52:              return R;
				case 0x53:              return S;
				case 0x54:              return T;
				case 0x55:              return U;
				case 0x56:              return V;
				case 0x57:              return W;
				case 0x58:              return X;
				case 0x59:              return Y;
				case 0x5A:              return Z;
				case VK_ESCAPE:         return ESC;
				case VK_F1:             return F1;
				case VK_F2:             return F2;
				case VK_F3:             return F3;
				case VK_F4:             return F4;
				case VK_F5:             return F5;
				case VK_F6:             return F6;
				case VK_F7:             return F7;
				case VK_F8:             return F8;
				case VK_F9:             return F9;
				case VK_F10:            return F10;
				case VK_F11:            return F11;
				case VK_F12:            return F12;
				case VK_F13:            return F13;
				case VK_F14:            return F14;
				case VK_F15:            return F15;
				case VK_F16:            return F16;
				case VK_F17:            return F17;
				case VK_F18:            return F18;
				case VK_F19:            return F19;
				case VK_F20:            return F20;
				case VK_F21:            return F21;
				case VK_F22:            return F22;
				case VK_F23:            return F23;
				case VK_F24:            return F24;
				case VK_UP:             return UP;
				case VK_DOWN:           return DOWN;
				case VK_LEFT:           return LEFT;
				case VK_RIGHT:          return RIGHT;
				case VK_LSHIFT:         return LEFT_SHIFT;
				case VK_RSHIFT:         return RIGHT_SHIFT;
				case VK_LCONTROL:       return LEFT_CTRL;
				case VK_RCONTROL:       return RIGHT_CTRL;
				case VK_LMENU:          return LEFT_ALT;
				case VK_RMENU:          return RIGHT_ALT;
				case VK_TAB:            return TAB;
				case VK_RETURN:         return ENTER;
				case VK_BACK:           return BACKSPACE;
				case VK_INSERT:         return INSERT;
				case VK_DELETE:         return DEL;
				case VK_PRIOR:          return PAGEUP;
				case VK_NEXT:           return PAGEDOWN;
				case VK_HOME:           return HOME;
				case VK_END:            return END;
				case VK_NUMPAD0:        return KP_0;
				case VK_NUMPAD1:        return KP_1;
				case VK_NUMPAD2:        return KP_2;
				case VK_NUMPAD3:        return KP_3;
				case VK_NUMPAD4:        return KP_4;
				case VK_NUMPAD5:        return KP_5;
				case VK_NUMPAD6:        return KP_6;
				case VK_NUMPAD7:        return KP_7;
				case VK_NUMPAD8:        return KP_8;
				case VK_NUMPAD9:        return KP_9;
				case VK_CAPITAL:        return CAPS_LOCK;
				case VK_SCROLL:         return SCROLL_LOCK;
				case VK_PAUSE:          return PAUSE;
				case VK_MENU:           return MENU;
				default:                return UNKNOWN;
            };
        }(),
            static_cast<uint16_t>(keyInput.VKey),
            [&]()
        {
            switch (keyInput.Flags)
            {
				case RI_KEY_MAKE:    return KeyEvent::STATE_PRESSED;
				case RI_KEY_BREAK:   return KeyEvent::STATE_RELEASED;
				default:             return KeyEvent::STATE_UNKNOWN;
            }
        }(),
            [&]()
        {
            KeyEvent::KeyModifiers modifiers;

            if (altDown)    modifiers.flip(KeyEvent::MODIFIER_ALT);
            if (shiftDown)  modifiers.flip(KeyEvent::MODIFIER_SHIFT);
            if (ctrlDown)   modifiers.flip(KeyEvent::MODIFIER_CONTROL);

            return modifiers;
        }()
            ));
    }

    void WindowManagerImpl::ProcessMouseInput(const RAWMOUSE& mouseInput)
    {
        if (!mMouseButtonCallback && !mMousePositionCallback)
            return;

		mCurrentMouseX += mouseInput.lLastX;
		mCurrentMouseY += mouseInput.lLastY;

        mMouseButtonEvents.push_back(MouseButtonEvent([&]()
        {
            switch (mouseInput.usButtonFlags)
            {
				case RI_MOUSE_LEFT_BUTTON_DOWN:     return MouseButtonEvent(MouseButtonEvent::BUTTON_LEFT, MouseButtonEvent::STATE_PRESSED);
				case RI_MOUSE_LEFT_BUTTON_UP:       return MouseButtonEvent(MouseButtonEvent::BUTTON_LEFT, MouseButtonEvent::STATE_RELEASED);
				case RI_MOUSE_MIDDLE_BUTTON_DOWN:   return MouseButtonEvent(MouseButtonEvent::BUTTON_MIDDLE, MouseButtonEvent::STATE_PRESSED);
				case RI_MOUSE_MIDDLE_BUTTON_UP:     return MouseButtonEvent(MouseButtonEvent::BUTTON_MIDDLE, MouseButtonEvent::STATE_RELEASED);
				case RI_MOUSE_RIGHT_BUTTON_DOWN:    return MouseButtonEvent(MouseButtonEvent::BUTTON_RIGHT, MouseButtonEvent::STATE_PRESSED);
				case RI_MOUSE_RIGHT_BUTTON_UP:      return MouseButtonEvent(MouseButtonEvent::BUTTON_RIGHT, MouseButtonEvent::STATE_RELEASED);
				default:                            return MouseButtonEvent(MouseButtonEvent::BUTTON_NONE, MouseButtonEvent::STATE_UNKNOWN);
            };
        }()));
    }
}