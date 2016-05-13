#include "include/Renderer/RenderQueue.h"

#include <limits>

namespace JonsEngine
{
    const RenderableSkeleton::BoneIndex RenderableSkeleton::INVALID_BONE_INDEX = std::numeric_limits<BoneIndex>::max();


    RenderableSkeleton::RenderableSkeleton() :
        RenderableSkeleton(INVALID_BONE_INDEX, INVALID_BONE_INDEX)
    {
    }
        
    RenderableSkeleton::RenderableSkeleton(const BoneIndex begin, const BoneIndex end) :
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
        mSkyboxTextureID = INVALID_DX11_MATERIAL_ID;
    
        mCamera.mModels.clear();
        mDirectionalLights.clear();
        mPointLights.clear();
        mBones.clear();
    }
}