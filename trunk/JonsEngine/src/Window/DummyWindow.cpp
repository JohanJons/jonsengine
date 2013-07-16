#include "include/Window/DummyWindow.h"

namespace JonsEngine
{
    DummyWindow::DummyWindow(const EngineSettings& engineSettings)
    {
    }
        
    DummyWindow::~DummyWindow()
    {
    }

        
    void DummyWindow::StartFrame()
    {
    }
        
    void DummyWindow::EndFrame()
    {
    }

        
    void DummyWindow::SetFrameLimit(const uint16_t newFrameLimit)
    {
    }
        
    uint16_t DummyWindow::GetFrameLimit() const
    {
        return 0;
    }
        
    uint16_t DummyWindow::GetCurrentFPS() const
    {
        return 0;
    }

        
    void DummyWindow::SetFullscreen(bool fullscreen)
    {
    }
        
    void DummyWindow::SetScreenResolution(const uint32_t width, const uint32_t height)
    {
    }
        
    void DummyWindow::SetWindowTitle(const std::string& windowTitle)
    {
    }
        
    void DummyWindow::SetFOV(const float FOV)
    {
    }

        
    bool DummyWindow::GetFullscreen() const
    {
        return false;
    }
        
    uint32_t DummyWindow::GetScreenWidth() const
    {
        return 0;
    }
        
    uint32_t DummyWindow::GetScreenHeight() const
    {
        return 0;
    }
        
    const std::string& DummyWindow::GetWindowTitle() const
    {
        return mWindowTitle;
    }
        
    float DummyWindow::GetFOV() const
    {
        return 0;
    }
}