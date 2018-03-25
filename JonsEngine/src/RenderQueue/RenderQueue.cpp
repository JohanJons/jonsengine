#include "include/RenderQueue/RenderQueue.h"

namespace JonsEngine
{
    RenderQueue::RenderQueue() : 
        mAmbientLight(0.0f),
        mSkyboxTextureID(INVALID_DX11_TEXTURE_ID)
    {
    }

    
    void RenderQueue::Clear()
    {
        mDirectionalLights.clear();
        mPointLights.clear();
		mTerrains.mTerrainData.clear();
		mTerrains.mTransforms.clear();

        mRenderData.mStaticMeshes.clear();
		mRenderData.mAnimatedMeshes.clear();
		mRenderData.mMaterials.clear();
		mRenderData.mBones.clear();

		mAmbientLight = Vec4(0.0f);
		mSkyboxTextureID = INVALID_DX11_TEXTURE_ID;

		mAABBRenderData.mRenderableAABBs.clear();
    }
}