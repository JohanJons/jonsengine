#include "include/RenderQueue/RenderQueue.h"

namespace JonsEngine
{
    RenderQueue::RenderQueue() : 
        mAmbientLight(0.0f),
        mSkyboxTextureID(INVALID_DX11_MATERIAL_ID)
    {
    }

    
    void RenderQueue::Clear()
    {
        mCameras.clear();
        mDirectionalLights.clear();
        mPointLight.clear();

        mMeshes.clear();
        mMaterials.clear();
        mBones.clear();
    }
}