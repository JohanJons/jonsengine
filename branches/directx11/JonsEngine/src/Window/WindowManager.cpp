#include "include/Window/WindowManager.h"

namespace JonsEngine
{
    WindowManager::WindowManager(const EngineSettings& engineSettings)
    {

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