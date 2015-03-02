#include "include/Renderer/RenderQueue.h"

namespace JonsEngine
{
    void RenderQueue::Clear()
    {
        mCamera.mModels.clear();
        mDirectionalLights.clear();
        mPointLights.clear();
    }
}