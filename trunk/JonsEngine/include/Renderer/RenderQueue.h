#pragma once

#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
    struct RenderableMesh
    {
        inline RenderableMesh(const MeshID mesh, const Mat4& wvpMatrix, const Mat4& worldMatrix) : mMeshID(mesh), mWVPMatrix(wvpMatrix), mWorldMatrix(worldMatrix)
        {
        }


        MeshID mMeshID;
        Mat4 mWVPMatrix;
        Mat4 mWorldMatrix;
    };
    
    typedef std::vector<RenderableMesh> RenderableMeshes;

    struct RenderableMaterial
    {
        Vec4 mDiffuseColor;
        Vec4 mAmbientColor;
        Vec4 mSpecularColor;
        Vec4 mEmissiveColor;

        TextureID mDiffuseTextureID;
        TextureID mNormalTextureID;
        float mSpecularFactor;
        float mTextureTilingFactor;
    };

    struct RenderableModel
    {
        RenderableMesh mMesh;
        RenderableMaterial mMaterial;
    };
    
    typedef std::vector<RenderableModel> RenderableModels;

    struct RenderableDirLight
    {
        Vec4 mLightColor;
        Vec3 mLightDirection;

        RenderableMeshes mMeshes;
    };
    
    typedef std::vector<RenderableDirLight> RenderableDirLights;

    struct RenderablePointLight
    {
        Mat4 mWVPMatrix;
        Mat4 mWorldMatrix;
        Vec4 mLightColor;
        Vec3 mLightPosition;

        float mLightIntensity;
        float mMaxDistance;

        RenderableMeshes mMeshes;
    };
    
    typedef std::vector<RenderablePointLight> RenderablePointLights;

    struct RenderQueue
    {
        void Clear();


        Mat4 mCameraViewMatrix;
        Mat4 mCameraProjectionMatrix;
        Vec4 mAmbientLight;
        Vec3 mCameraPosition;
        float mFOV;

        // TODO: more than one camera?
        RenderableModels mVisibleModels;
        RenderableDirLights mDirectionalLights;
        RenderablePointLights mPointLights;
    };
}