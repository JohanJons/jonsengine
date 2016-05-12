#include "include/Renderer/RenderQueue.h"

namespace JonsEngine
{
    RenderableSkeleton() : 
        RenderableSkeleton(INVALID_BONE_INDEX, INVALID_BONE_INDEX)
    {
    }
        
    RenderableSkeleton(const BoneIndex begin, const BoneIndex end) :
        mBoneIndexBegin(begin),
        mBoneIndexEnd(end)
    {
    }
    
    RenderQueue::RenderQueue() :
        mAmbientLight(0.01f),
        mSkyboxTextureID(INVALID_DX11_MATERIAL_ID)
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