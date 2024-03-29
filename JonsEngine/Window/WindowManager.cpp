#include "Window/WindowManager.h"

#include "Core/EngineSettings.h"
#include "Core/Logging/Logger.h"
#include "Core/Platform.h"

#include <exception>
#include <functional>


namespace JonsEngine
{
	struct NativeResources
	{
		HINSTANCE mInstanceHandle;
		HWND mWindowHandle;
	};

	static std::function<void(const RAWKEYBOARD& keyInput)> gKeyboardCallback = nullptr;
	static std::function<void(const RAWMOUSE& mouseInput)> gMouseCallback = nullptr;

	const LONG gWindowedStyle = WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME;


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
			assert(gKeyboardCallback);
			assert(gMouseCallback);

			RAWINPUT rawInput;
			UINT rawInputSize = sizeof(RAWINPUT);

			GetRawInputData((HRAWINPUT)(lParam), RID_INPUT, &rawInput, &rawInputSize, sizeof(RAWINPUTHEADER));

			if (rawInput.header.dwType == RIM_TYPEKEYBOARD)
				gKeyboardCallback(rawInput.data.keyboard);
			else if (rawInput.header.dwType == RIM_TYPEMOUSE)
				gMouseCallback(rawInput.data.mouse);

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


	WindowManager::WindowManager(const WindowSettings& settings, Logger& logger) :
		mLogger(logger),

		mWindowTitle(settings.mWindowTitle),
		mWindowDimensions(settings.mWindowDimensions),
		mShowMouseCursor(false),
		mFullscreen(false),

		mMouseMovement({0, 0}),
		mMouseButtonCallback(nullptr),
		mMouseMovementCallback(nullptr),
		mKeyCallback(nullptr),

		mNativeHandles(new NativeResources({ GetModuleHandle(NULL), nullptr }))
	{
		// not foolproof but should work
		assert(SetProcessDPIAware());
		// only one instance supported atm
		assert(!gKeyboardCallback && !gMouseCallback);

		// Register class
		WNDCLASSEX wcex;
		ZeroMemory(&wcex, sizeof(WNDCLASSEX));
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = mNativeHandles->mInstanceHandle;
		wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wcex.hCursor = NULL;
		wcex.lpszMenuName = mWindowTitle.c_str();
		wcex.lpszClassName = mWindowTitle.c_str();
		wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;
		if (!RegisterClassEx(&wcex))
		{
			const std::string win32Error = GetWin32ErrorString();
			JONS_LOG_ERROR(mLogger, "WindowManager::WindowManager(): RegisterClassEx failed: " + win32Error);
			throw std::runtime_error("WindowManager::WindowManager(): RegisterClassEx failed: " + win32Error);
		}

		// Create window
		RECT windowRect = { 0, 0, static_cast<LONG>(mWindowDimensions.x), static_cast<LONG>(mWindowDimensions.y) };
		AdjustWindowRectEx(&windowRect, gWindowedStyle, FALSE, WS_EX_APPWINDOW);
		mNativeHandles->mWindowHandle = CreateWindow(wcex.lpszClassName, wcex.lpszClassName, gWindowedStyle, CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, mNativeHandles->mInstanceHandle, NULL);
		if (!mNativeHandles->mWindowHandle)
		{
			const std::string win32Error = GetWin32ErrorString();
			JONS_LOG_ERROR(mLogger, "WindowManager::WindowManager(): CreateWindow failed: " + win32Error);
			throw std::runtime_error("WindowManager::WindowManager(): CreateWindow failed: " + win32Error);
		}

		ShowCursor(mShowMouseCursor);
		ShowWindow(mNativeHandles->mWindowHandle, SW_SHOWNORMAL);
		UpdateWindow(mNativeHandles->mWindowHandle);

		// initialize RAW input
		RAWINPUTDEVICE rawInputDevices[2] =
		{
			{ 0x01, 0x02, RIDEV_NOLEGACY, mNativeHandles->mWindowHandle },  // mouse
			{ 0x01, 0x06, RIDEV_NOLEGACY, mNativeHandles->mWindowHandle }   // keyboard
		};

		if (!RegisterRawInputDevices(rawInputDevices, 2, sizeof(RAWINPUTDEVICE)))
		{
			const std::string win32Error = GetWin32ErrorString();
			JONS_LOG_ERROR(mLogger, "WindowManager::WindowManager(): RegisterRawInputDevices failed: " + win32Error);
			throw std::runtime_error("WindowManager::WindowManager(): RegisterRawInputDevices failed: " + win32Error);
		}

		if (settings.mFullscreen)
			SetFullscreen(true);

		using namespace std::placeholders;
		gKeyboardCallback = std::bind(&WindowManager::ProcessKeyboardInput, this, _1);
		gMouseCallback = std::bind(&WindowManager::ProcessMouseInput, this, _1);
	}

	WindowManager::~WindowManager()
	{
		DestroyWindow(mNativeHandles->mWindowHandle);
		gKeyboardCallback = nullptr;
		gMouseCallback = nullptr;
	}


	void WindowManager::Poll()
	{
		MSG msg = { 0 };
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Dispatch waiting events for each event type to registered callbacks
		if (mMouseMovementCallback)
		{
			mMouseMovementCallback(mMouseMovement);
			mMouseMovement = { 0, 0 };
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

	void WindowManager::SetMouseButtonCallback()
	{
		mMouseButtonCallback = nullptr;
	}

	void WindowManager::SetMouseButtonCallback(const MouseButtonCallback& callback)
	{
		mMouseButtonCallback = callback;
	}

	void WindowManager::SetMouseMovementCallback()
	{
		mMouseMovementCallback = nullptr;
	}

	void WindowManager::SetMouseMovementCallback(const MouseMovementCallback& callback)
	{
		mMouseMovementCallback = callback;
	}

	void WindowManager::SetKeyCallback()
	{
		mKeyCallback = nullptr;
	}

	void WindowManager::SetKeyCallback(const KeyCallback& callback)
	{
		mKeyCallback = callback;
	}

	void WindowManager::ShowMouseCursor(const bool show)
	{
		if (show)
			while (ShowCursor(true) <= 0);
		else
			while (ShowCursor(false) >= 0);

		SendMessage(mNativeHandles->mWindowHandle, WM_SETCURSOR, (WPARAM)mNativeHandles->mWindowHandle, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));     // needed?

		UpdateWindow(mNativeHandles->mWindowHandle);

		mShowMouseCursor = show;
	}

	void WindowManager::SetMousePosition(const uint32_t x, const uint32_t y)
	{
		POINT cursorPos = { static_cast<LONG>(x), static_cast<LONG>(y) };

		ClientToScreen(mNativeHandles->mWindowHandle, &cursorPos);
		SetCursorPos(cursorPos.x, cursorPos.y);

		UpdateWindow(mNativeHandles->mWindowHandle);
	}


	void WindowManager::SetFullscreen(const bool fullscreen)
	{
		if (mFullscreen == fullscreen)
			return;

		SendMessage(mNativeHandles->mWindowHandle, WM_JONS_FULLSCREEN, fullscreen, mWindowDimensions.x | (mWindowDimensions.y << 16));

		mFullscreen = fullscreen;
	}

	void WindowManager::SetScreenResolution(const uint32_t width, const uint32_t height)
	{
		SetScreenResolution({ width, height });
	}

	void WindowManager::SetScreenResolution(const WindowDimensions& dimensions)
	{
		SendMessage(mNativeHandles->mWindowHandle, WM_JONS_RESIZE, mFullscreen, mWindowDimensions.x | (mWindowDimensions.y << 16));

		mWindowDimensions = dimensions;
	}

	void WindowManager::SetWindowTitle(const std::string& windowTitle)
	{
		mWindowTitle = windowTitle;

		SetWindowText(mNativeHandles->mWindowHandle, mWindowTitle.c_str());

		UpdateWindow(mNativeHandles->mWindowHandle);
	}

	bool WindowManager::GetFullscreen() const
	{
		return mFullscreen;
	}

	WindowDimensions WindowManager::GetWindowDimensions() const
	{
		return mWindowDimensions;
	}

	WindowPosition WindowManager::GetCurrentMousePosition() const
	{
		POINT p;
		ZeroMemory(&p, sizeof(POINT));
		assert(GetCursorPos(&p));

		// note: screen, not clientwindow coordinates!
		return{ p.x, p.y };
	}

	const std::string& WindowManager::GetWindowTitle() const
	{
		return mWindowTitle;
	}

	bool WindowManager::GetShowMouseCursor() const
	{
		return mShowMouseCursor;
	}


	void WindowManager::ProcessKeyboardInput(const tagRAWKEYBOARD& keyInput)
	{
		// '255' means fake/illegal key press
		if (!mKeyCallback || keyInput.VKey == 255)
			return;

		const bool isE0 = ((keyInput.Flags & RI_KEY_E0) != 0);

		// key modifiers
		if (keyInput.VKey == VK_CONTROL)
			mKeyboardData.mCtrlPressed = keyInput.Flags == RI_KEY_MAKE;
		if (keyInput.VKey == VK_MENU)
			mKeyboardData.mAltPressed = keyInput.Flags == RI_KEY_MAKE;
		if (keyInput.VKey == VK_SHIFT)
			mKeyboardData.mShiftPressed = keyInput.Flags == RI_KEY_MAKE;

		mKeyEvents.emplace_back(KeyEvent(
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
			case VK_SHIFT:          return isE0 ? RIGHT_SHIFT : LEFT_SHIFT;
			case VK_CONTROL:        return isE0 ? RIGHT_CTRL : LEFT_CTRL;
			case VK_MENU:           return isE0 ? RIGHT_ALT : LEFT_ALT;
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
			default:                return UNKNOWN;
			};
		}(),
			static_cast<uint16_t>(keyInput.VKey),
			[&]()
		{
			switch (keyInput.Flags)
			{
			default:
			case RI_KEY_MAKE:    return KeyEvent::KeyState::STATE_PRESSED;
			case RI_KEY_BREAK:   return KeyEvent::KeyState::STATE_RELEASED;
			}
		}(),
			mKeyboardData.mCtrlPressed,
			mKeyboardData.mAltPressed,
			mKeyboardData.mShiftPressed
			));
	}

	void WindowManager::ProcessMouseInput(const tagRAWMOUSE& mouseInput)
	{
		if (!mMouseButtonCallback && !mMouseMovementCallback)
			return;

		// make sure mouse movements are relative
		assert(!(mouseInput.usFlags & (1 << 0)));

		mMouseMovement.x += mouseInput.lLastX;
		mMouseMovement.y += mouseInput.lLastY;

		if (!mouseInput.usButtonFlags)
			return;

		mMouseButtonEvents.emplace_back(MouseButtonEvent([&]()
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