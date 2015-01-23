#include "include/Renderer/RenderQueue.h"

namespace JonsEngine
{
    void RenderQueue::Clear()
    {
        mDirectionalLights.clear();
        mPointLights.clear();
    }
}