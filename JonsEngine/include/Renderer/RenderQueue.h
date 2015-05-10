#pragma once

#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
    struct RenderableMesh
    {
        inline RenderableMesh(const MeshID mesh, const Mat4& worldMatrix) : mMeshID(mesh), mWorldMatrix(worldMatrix)
        {
        }


        MeshID mMeshID;
        Mat4 mWorldMatrix;
    };
    
    typedef std::vector<RenderableMesh> RenderableMeshes;

    struct RenderableMaterial
    {
        inline RenderableMaterial(const TextureID diffuseTexture, const TextureID normalTexture, const float specFactor)
            : mDiffuseTextureID(diffuseTexture), mNormalTextureID(normalTexture), mSpecularFactor(specFactor)
        {
        }

        TextureID mDiffuseTextureID;
        TextureID mNormalTextureID;
        float mSpecularFactor;
    };

    struct RenderableModel
    {
        inline RenderableModel(const MeshID mesh, const Mat4& worldMatrix, const TextureID diffuseTexture, const TextureID normalTexture, const float specFactor, const float tilingFactor) :
            mMesh(mesh, worldMatrix), mMaterial(diffuseTexture, normalTexture, specFactor), mTextureTilingFactor(tilingFactor)
        {
        }

        RenderableMesh mMesh;
        RenderableMaterial mMaterial;
        float mTextureTilingFactor;
    };

    typedef std::vector<RenderableModel> RenderableModels;

    struct RenderableCamera
    {
		Mat4 mCameraViewMatrix;
        Mat4 mCameraProjectionMatrix;
        Mat4 mCameraViewProjectionMatrix;
		Vec3 mCameraPosition;
        float mFOV;

        RenderableModels mModels;
    };

    struct RenderableDirLight
    {
        inline RenderableDirLight(const Vec4& lightColor, const Vec3& lightDir) : mLightColor(lightColor), mLightDirection(lightDir)
        {
        }


        Vec4 mLightColor;
        Vec3 mLightDirection;

        RenderableMeshes mMeshes;
    };
    
    typedef std::vector<RenderableDirLight> RenderableDirLights;

    struct RenderablePointLight
    {
        inline RenderablePointLight(const Vec4& lightColor, const Vec3& lightPosition, const float intensity, const float radius) :
            mLightColor(lightColor), mLightPosition(lightPosition), mLightIntensity(intensity), mLightRadius(radius)
        {
        }


        Vec4 mLightColor;
        Vec3 mLightPosition;

        float mLightIntensity;
        float mLightRadius;

        RenderableMeshes mMeshes;
    };
    
    typedef std::vector<RenderablePointLight> RenderablePointLights;

    struct RenderQueue
    {
        void Clear();


        Vec4 mAmbientLight;

        // TODO: more than one camera?
        RenderableCamera mCamera;
        RenderableDirLights mDirectionalLights;
        RenderablePointLights mPointLights;
    };
}