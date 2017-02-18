#pragma once

#include "include/RenderQueue/RenderableCamera.h"
#include "include/RenderQueue/RenderableDirectionalLight.h"
#include "include/RenderQueue/RenderablePointLight.h"
#include "include/RenderQueue/RenderableTerrain.h"
#include "include/RenderQueue/RenderableMaterial.h"
#include "include/RenderQueue/RenderableMesh.h"
#include "include/RenderQueue/AABBRenderData.h"
#include "include/Resources/Bone.h"

#include <vector>

namespace JonsEngine
{
    struct RenderQueue
    {
        typedef std::vector<RenderablePointLight> RenderablePointLights;
        typedef std::vector<RenderableDirectionalLight> RenderableDirectionalLights;
		typedef std::vector<RenderableTerrain> RenderableTerrains;

		struct RenderData
		{
			RenderableMeshContainer mStaticMeshes;
			RenderableMeshContainer mAnimatedMeshes;
			RenderableMaterial::ContainerType mMaterials;
			BoneTransforms mBones;
		};

        RenderQueue();

        void Clear();



        Vec4 mAmbientLight;
        DX11TextureID mSkyboxTextureID;

        RenderableCamera mCamera;
        RenderablePointLights mPointLights;
        RenderableDirectionalLights mDirectionalLights;
		RenderableTerrains mTerrains;
		RenderData mRenderData;

		// debug-related stuff
		AABBRenderData mAABBRenderData;
    };
}