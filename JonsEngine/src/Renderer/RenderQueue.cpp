#include "include/Renderer/RenderQueue.h"

namespace JonsEngine
{
    RenderQueue::RenderQueue() : mAmbientLight(0.01f), mSkyboxTextureID(INVALID_DX11_MATERIAL_ID)
    {
    }

    void RenderQueue::Clear()
    {
        mCamera.mModels.clear();
        mDirectionalLights.clear();
        mPointLights.clear();
        mSkyboxTextureID = INVALID_DX11_MATERIAL_ID;
    }
}