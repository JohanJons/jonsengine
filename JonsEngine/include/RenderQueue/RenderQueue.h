#pragma once

#include "include/RenderQueue/RenderableCamera.h"
#include "include/RenderQueue/RenderableDirectionalLight.h"
#include "include/RenderQueue/RenderablePointLight.h"
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
        DX11MaterialID mSkyboxTextureID;

        RenderableCamera mCamera;
        RenderablePointLights mPointLights;
        RenderableDirectionalLights mDirectionalLights;
		RenderData mRenderData;

		// debug-related stuff
		AABBRenderData mAABBRenderData;
    };
}