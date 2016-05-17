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
        typedef std::vector<RenderableCamera> RenderableCameras;
        typedef std::vector<RenderablePointLight> RenderablePointLights;
        typedef std::vector<RenderableDirectionalLight> RenderableDirectionalLights;

        RenderQueue();

        void Clear();


        Vec4 mAmbientLight;
        DX11MaterialID mSkyboxTextureID;

        RenderableCameras mCameras;
        RenderablePointLights mPointLight;
        RenderableDirectionalLights mDirectionalLights;

        RenderableMesh::Collection mMeshes;
        RenderableMaterial::Collection mMaterials;
        RenderableBone::Collection mBones;
    };
}