#pragma once

#include "include/RenderQueue/RenderableCamera.h"
#include "include/RenderQueue/RenderableDirectionalLight.h"
#include "include/RenderQueue/RenderablePointLight.h"
#include "include/RenderQueue/RenderableMaterial.h"
#include "include/RenderQueue/RenderableMesh.h"
#include "include/RenderQueue/RenderableBone.h"

#include <vector>

namespace JonsEngine
{
    struct RenderQueue
    {
        typedef std::vector<RenderablePointLight> RenderablePointLights;
        typedef std::vector<RenderableDirectionalLight> RenderableDirectionalLights;

		struct RenderData
		{
			RenderableMesh::ContainerType mMeshes;
			RenderableMaterial::ContainerType mMaterials;
			RenderableBone::ContainerType mBones;
		};

        RenderQueue();

        void Clear();


        Vec4 mAmbientLight;
        DX11MaterialID mSkyboxTextureID;

        RenderableCamera mCamera;
        RenderablePointLights mPointLights;
        RenderableDirectionalLights mDirectionalLights;

		RenderData mRenderData;
    };
}