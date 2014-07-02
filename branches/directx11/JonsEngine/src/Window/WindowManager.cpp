#include "include/Window/WindowManager.h"

#include "include/Window/WindowManagerImpl.h"
#include "include/Core/EngineSettings.h"


namespace JonsEngine
{
    WindowManager::WindowManager(const EngineSettings& engineSettings, IMemoryAllocatorPtr memoryAllocator, Logger& logger) :
        mLogger(logger), mMemoryAllocator(memoryAllocator),
        mImplementation(mMemoryAllocator->AllocateObject<WindowManagerImpl>(engineSettings, mLogger), [this](WindowManagerImpl* windowMgrImpl) { mMemoryAllocator->DeallocateObject(windowMgrImpl); })
    {
    }
    
    WindowManager::~WindowManager()
    {
    }
    
    void WindowManager::Poll()
    {
        mImplementation->Poll();
    }
    
    void WindowManager::SetMouseButtonCallback()
    {
        mImplementation->SetMouseButtonCallback();
    }
    
    void WindowManager::SetMouseButtonCallback(const MouseButtonCallback& callback)
    {
        mImplementation->SetMouseButtonCallback(callback);
    }
    
    void WindowManager::SetMousePositionCallback()
    {
        mImplementation->SetMousePositionCallback();
    }
    
    void WindowManager::SetMousePositionCallback(const MousePositionCallback& callback)
    {
        mImplementation->SetMousePositionCallback(callback);
    }
    
    void WindowManager::SetKeyCallback()
    {
        mImplementation->SetKeyCallback();
    }
    
    void WindowManager::SetKeyCallback(const KeyCallback& callback)
    {
        mImplementation->SetKeyCallback(callback);
    }

    void WindowManager::ShowMouseCursor(const bool show)
    {
        mImplementation->ShowMouseCursor(show);
    }

    void WindowManager::SetMousePosition(const uint32_t x, const uint32_t y)
    {
        mImplementation->SetMousePosition(x, y);
    }


    void WindowManager::SetFullscreen(const bool fullscreen)
    {
        mImplementation->SetFullscreen(fullscreen);
    }

    void WindowManager::SetScreenResolution(const uint32_t width, const uint32_t height)
    {
        mImplementation->SetScreenResolution(width, height);
    }

    void WindowManager::SetWindowTitle(const std::string& windowTitle)
    {
        mImplementation->SetWindowTitle(windowTitle);
    }

    void WindowManager::SetFOV(const float FOV)
    {
        mImplementation->SetFOV(FOV);
    }

    bool WindowManager::GetFullscreen() const
    {
        return mImplementation->GetFullscreen();
    }

    uint32_t WindowManager::GetScreenWidth() const
    {
        return mImplementation->GetScreenWidth();
    }

    uint32_t WindowManager::GetScreenHeight() const
    {
        return mImplementation->GetScreenHeight();
    }

    const std::string& WindowManager::GetWindowTitle() const
    {
        return mImplementation->GetWindowTitle();
    }

    bool WindowManager::GetShowMouseCursor() const
    {
        return mImplementation->GetShowMouseCursor();
    }

    float WindowManager::GetFOV() const
    {
        return mImplementation->GetFOV();
    }
}