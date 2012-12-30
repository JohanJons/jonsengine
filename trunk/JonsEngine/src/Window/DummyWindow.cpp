#include "include/Window/DummyWindow.h"

#include "include/Core/EngineSettings.h"

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

        
    bool DummyWindow::SetupWindow()
    {
        return false;
    }
        
    void DummyWindow::CloseWindow()
    { 
    }
        
    bool DummyWindow::IsWindowOpened() const
    {
        return false;
    }
        
    uint16_t DummyWindow::GetCurrentFPS() const
    {
        return 0;    
    }

        
    void DummyWindow::SetFullscreen(bool fullscreen)
    {
    }
        
    void DummyWindow::SetScreenResolution(const uint16_t width, const uint16_t height)
    {
    }
        
    void DummyWindow::SetWindowTitle(const std::string& windowTitle)
    {
    }

        
    const ScreenMode& DummyWindow::GetScreenMode() const          { return mScreenMode;   }
    const std::string& DummyWindow::GetWindowTitle() const        { return mWindowTitle;  }
}