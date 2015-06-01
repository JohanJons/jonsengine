#pragma once

#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
    struct RenderableMesh
    {
        RenderableMesh(const MeshID mesh, const IDMap<Mat4>::ItemID worldTransformID, const IDMap<Mat4>::ItemID localTransformID) :
            mMeshID(mesh), mWorldTransformID(worldTransformID), mLocalTransformID(localTransformID)
        {
        }


        MeshID mMeshID;
        IDMap<Mat4>::ItemID mWorldTransformID;
        IDMap<Mat4>::ItemID mLocalTransformID;
    };
    
    typedef std::vector<RenderableMesh> RenderableMeshes;

    struct RenderableMaterial
    {
        RenderableMaterial(const TextureID diffuseTexture, const TextureID normalTexture, const float specFactor)
            : mDiffuseTextureID(diffuseTexture), mNormalTextureID(normalTexture), mSpecularFactor(specFactor)
        {
        }

        TextureID mDiffuseTextureID;
        TextureID mNormalTextureID;
        float mSpecularFactor;
    };

    struct RenderableModel
    {
        RenderableModel(const MeshID mesh, const IDMap<Mat4>::ItemID worldTransformID, const IDMap<Mat4>::ItemID localTransformID, const TextureID diffuseTexture, const TextureID normalTexture, const float specFactor, const float tilingFactor) :
            mMesh(mesh, worldTransformID, localTransformID), mMaterial(diffuseTexture, normalTexture, specFactor), mTextureTilingFactor(tilingFactor)
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
        RenderableDirLight(const Vec4& lightColor, const Vec3& lightDir) : mLightColor(lightColor), mLightDirection(lightDir)
        {
        }


        Vec4 mLightColor;
        Vec3 mLightDirection;

        RenderableMeshes mMeshes;
    };
    
    typedef std::vector<RenderableDirLight> RenderableDirLights;

    struct RenderablePointLight
    {
        RenderablePointLight(const Vec4& lightColor, const Vec3& lightPosition, const float intensity, const float radius) :
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
        RenderQueue(const IDMap<Mat4>& localTransformStorage, const IDMap<Mat4>& worldTransformStorage) :
            mLocalTransformStorage(localTransformStorage), mWorldTransformStorage(worldTransformStorage)
        {
        }

        void Clear();


        Vec4 mAmbientLight;

        // TODO: more than one camera?
        RenderableCamera mCamera;
        RenderableDirLights mDirectionalLights;
        RenderablePointLights mPointLights;

        const IDMap<Mat4>& mLocalTransformStorage;
        const IDMap<Mat4>& mWorldTransformStorage;
    };
}