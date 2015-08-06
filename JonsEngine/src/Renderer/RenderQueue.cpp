#include "include/Renderer/RenderQueue.h"

namespace JonsEngine
{
    RenderQueue::RenderQueue(const IDMap<Mat4>& localTransformStorage, const IDMap<Mat4>& worldTransformStorage) :
        mAmbientLight(0.01f), mSkyboxTextureID(INVALID_DX11_MATERIAL_ID), mLocalTransformStorage(localTransformStorage), mWorldTransformStorage(worldTransformStorage)
    {
    }

    void RenderQueue::Clear()
    {
        mCamera.mModels.clear();
        mDirectionalLights.clear();
        mPointLights.clear();
    }
}