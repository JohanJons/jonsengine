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

    struct RenderableDirLight
    {
        Vec4 mLightColor;
        Vec3 mLightDirection;

        std::vector<RenderableMesh> mMeshes;
    };

    struct RenderablePointLight
    {
        Mat4 mWVPMatrix;
        Mat4 mWorldMatrix;
        Vec4 mLightColor;
        Vec3 mLightPosition;

        float mLightIntensity;
        float mMaxDistance;

        std::vector<RenderableMesh> mMeshes;
    };

    struct RenderQueue
    {
        void Clear();


        Mat4 mCameraViewMatrix;
        Mat4 mCameraProjectionMatrix;
        Vec4 mAmbientLight;
        Vec3 mCameraPosition;
        float mFOV;

        // TODO: more than one camera?
        std::vector<RenderableModel> mCamera;
        std::vector<RenderableDirLight> mDirectionalLights;
        std::vector<RenderablePointLight> mPointLights;
    };
}