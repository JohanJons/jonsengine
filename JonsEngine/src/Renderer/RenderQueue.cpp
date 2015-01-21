#include "include/Renderer/RenderQueue.h"

namespace JonsEngine
{
    void RenderQueue::Clear()
    {
        mVisibleModels.clear();
        mDirectionalLights.clear();
        mPointLights.clear();
    }
}