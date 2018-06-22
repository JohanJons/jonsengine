#include "include/RenderQueue/RenderQueue.h"

namespace JonsEngine
{
    RenderQueue::RenderQueue() : 
        mAmbientLight(0.0f),
        mSkyboxTextureID(INVALID_DX11_TEXTURE_ID)
    {
    }

    
    void RenderQueue::PerFrameClear()
    {
        mDirectionalLights.clear();
        mPointLights.clear();

        mRenderData.mStaticMeshes.clear();
		mRenderData.mAnimatedMeshes.clear();
		mRenderData.mMaterials.clear();
		mRenderData.mBones.clear();

		mAmbientLight = Vec4(0.0f);
		mSkyboxTextureID = INVALID_DX11_TEXTURE_ID;

		mAABBRenderData.clear();
		mColorsToAABBsList.clear();
    }

	void RenderQueue::AddAABB( const Mat4& transform, DX11MeshID mesh, Color color )
	{
		auto iter = std::find_if( mColorsToAABBsList.begin(), mColorsToAABBsList.end(), [ &color ]( const AABBsOfColor& ColorAABBMap ) { return ColorAABBMap.first == color; } );
		if ( iter == mColorsToAABBsList.end() )
		{
			mColorsToAABBsList.emplace_back( color );
			AABBsOfColor& colorAABBs = mColorsToAABBsList.back();
			colorAABBs.second.emplace_back( transform, mesh );
		}
		else
		{
			iter->second.emplace_back( transform, mesh );
		}
	}
}