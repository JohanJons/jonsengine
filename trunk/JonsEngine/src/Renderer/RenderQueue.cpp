#include "include/Renderer/RenderQueue.h"

namespace JonsEngine
{
    void RenderQueue::Clear()
    {
        mCamera.clear();
        mDirectionalLights.clear();
        mPointLights.clear();
    }
}