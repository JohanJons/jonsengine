#pragma once

#include "include/RenderQueue/RenderQueueTypes.h"
#include "include/Resources/Bone.h"

#include <vector>

namespace JonsEngine
{
    struct RenderQueue
    {
		RenderQueue();

		void PerFrameClear();

        Vec4 mAmbientLight;
        DX11TextureID mSkyboxTextureID;

        RenderableCamera mCamera;
        RenderablePointLights mPointLights;
        RenderableDirectionalLights mDirectionalLights;
		RenderableTerrains mTerrains;
		RenderData mRenderData;

		// debug-related stuff
		std::vector<AABBsOfColor> mColorsToAABBsList;
    };
}