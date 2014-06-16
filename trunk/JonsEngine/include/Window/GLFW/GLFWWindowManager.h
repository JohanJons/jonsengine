#pragma once

#include "include/Window/KeyCallback.h"
#include "include/Window/KeyDefinition.h"
#include "include/Window/MouseCallback.h"

#include <string>
#include <vector>

struct GLFWwindow;

namespace JonsEngine
{
    struct EngineSettings;
    class Logger;

    /* GLFWWindow definition */
    class GLFWWindowManager
    {
    public:
        typedef std::function<void (void)> OnContextCreatedCallback;

        GLFWWindowManager(const EngineSettings& engineSettings, OnContextCreatedCallback contextCreatedCallback);
        ~GLFWWindowManager();

        /*
        * Dispatches queued input events to all registered callbacks.
        * Is called by the Engine once per frame.
        */
        void Poll();
        void SwapColorBuffers();

        void SetMouseButtonCallback();
        void SetMouseButtonCallback(const MouseButtonCallback& callback);
        void SetMouseMotionCallback();
        void SetMouseMotionCallback(const MouseMotionCallback& callback);
		void SetMousePositionCallback();
        void SetMousePositionCallback(const MousePositionCallback& callback);
        void SetKeyCallback();
        void SetKeyCallback(const KeyCallback& callback);

        void ShowMouseCursor(bool show);
        void SetMousePosition(uint32_t x, uint32_t y);

        void SetFrameLimit(const uint64_t newFrameLimit);
        uint64_t GetFrameLimit() const;
        uint64_t GetCurrentFPS() const;

        void SetFullscreen(bool fullscreen);
        void SetScreenResolution(const uint32_t width, const uint32_t height);
        void SetWindowTitle(const std::string& windowTitle);
        void SetFOV(const float FOV);

        bool GetFullscreen() const;
        uint32_t GetScreenWidth() const;
        uint32_t GetScreenHeight() const;
        const std::string& GetWindowTitle() const;
        float GetFOV() const;
    

    private:
        bool SetupWindow();
        void InitializeGLEW();
        void ClearAllEvents();
        static void glfwOnWindowChanged(::GLFWwindow* window, int width, int height);
        static void glfwOnMouseButton(GLFWwindow* window, int button, int action, int mods);
        static void glfwOnMouseMotion(GLFWwindow* window, double posX, double posY);
        static void glfwOnKey(GLFWwindow* window, int key, int scancode, int action, int mods);

        OnContextCreatedCallback mOnContextCreated;

        Logger& mLogger;

        // FPS
        uint64_t mFrameLimit;
        double mStartFrameTime;
        double mLastFrameTime;
        double mThisFrameTime;

        GLFWwindow* mWindow;
        uint32_t mScreenWidth;
        uint32_t mScreenHeight;
        bool mShowMouseCursor;
        bool mFullscreen;
        float mFOV;
        std::string mWindowTitle;
		double mPreviousMouseX;
        double mPreviousMouseY;

        MouseButtonCallback mMouseButtonCallback;
        MouseMotionCallback mMouseMotionCallback;
		MousePositionCallback mMousePositionCallback;
        KeyCallback mKeyCallback;

        std::vector<MouseButtonEvent> mMouseButtonEvents;
        std::vector<MouseMotionEvent> mMouseMotionEvents;
        std::vector<KeyEvent>         mKeyEvents;
    };


    /* GLFWWindow inlines */
    inline uint64_t GLFWWindowManager::GetFrameLimit() const                                 { return mFrameLimit;  }
    inline bool GLFWWindowManager::GetFullscreen() const                                     { return mFullscreen;  }
    inline float GLFWWindowManager::GetFOV() const                                           { return mFOV;         }
    inline const std::string& GLFWWindowManager::GetWindowTitle() const                      { return mWindowTitle; }
}