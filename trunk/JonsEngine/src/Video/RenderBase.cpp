#include "include/Video/RenderBase.h"

namespace JonsEngine
{
    /////////////////////////////////////////////////////
    /////               ScreenMode                  /////
    /////////////////////////////////////////////////////

    ScreenMode::ScreenMode() : ScreenWidth(800), ScreenHeight(600), FrameLimit(60), FrameLimitEnabled(true), Fullscreen(false)
    {
    }



    /////////////////////////////////////////////////
    /////               RenderBase              /////
    /////////////////////////////////////////////////

    RenderBase::RenderBase()  : mScreenMode(), mWindowTitle("JonsEngine Game")
    {
    }
        
    RenderBase::~RenderBase()
    {
    }
}