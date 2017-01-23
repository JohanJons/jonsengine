#pragma once

#include "include/Window/KeyCallback.h"
#include "include/Window/KeyDefinition.h"
#include "include/Window/MouseCallback.h"

#include <string>
#include <vector>
#include <memory>

struct tagRAWKEYBOARD;
struct tagRAWMOUSE;

namespace JonsEngine
{
    struct WindowSettings;
	struct NativeResources;
    class Logger;

    class WindowManager
    {
    public:
		typedef iVec2 MousePosition;
		typedef uVec2 WindowDimensions;

        WindowManager(const WindowSettings& settings, Logger& logger);
        ~WindowManager();

        /*
        * Dispatches queued input events to all registered callbacks.
        * Is called by the Engine once per frame.
        */
        void Poll();

        void SetMouseButtonCallback();
        void SetMouseButtonCallback(const MouseButtonCallback& callback);
        void SetMousePositionCallback();
        void SetMousePositionCallback(const MousePositionCallback& callback);
        void SetKeyCallback();
        void SetKeyCallback(const KeyCallback& callback);

        void ShowMouseCursor(bool show);
        void SetMousePosition(uint32_t x, uint32_t y);

        void SetFullscreen(bool fullscreen);
        void SetScreenResolution(const uint32_t width, const uint32_t height);
		void SetScreenResolution(const WindowDimensions& dimensions);
        void SetWindowTitle(const std::string& windowTitle);

        bool GetFullscreen() const;
		WindowDimensions GetWindowDimensions() const;
		MousePosition GetCurrentMousePosition() const;
        const std::string& GetWindowTitle() const;
        bool GetShowMouseCursor() const;


	private:
		struct KeyboardData
		{
			bool mCtrlPressed;
			bool mAltPressed;
			bool mShiftPressed;

			inline KeyboardData() :
				mCtrlPressed(false),
				mAltPressed(false),
				mShiftPressed(false)
			{
			}
		};

    private:
		void ProcessKeyboardInput(const tagRAWKEYBOARD& keyInput);
		void ProcessMouseInput(const tagRAWMOUSE& mouseInput);

		Logger& mLogger;

		std::string mWindowTitle;
		WindowDimensions mWindowDimensions;
		bool mShowMouseCursor;
		bool mFullscreen;

		MousePosition mRelativePos;
		KeyboardData mKeyboardData;
		MouseButtonCallback mMouseButtonCallback;
		MousePositionCallback mMousePositionCallback;
		KeyCallback mKeyCallback;

		std::vector<MouseButtonEvent> mMouseButtonEvents;
		std::vector<KeyEvent> mKeyEvents;

		std::unique_ptr<NativeResources> mNativeHandles;
    };
}