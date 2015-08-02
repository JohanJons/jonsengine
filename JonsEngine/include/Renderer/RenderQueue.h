#pragma once

#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.hpp"

#include <vector>

namespace JonsEngine
{
    struct RenderableMesh
    {
        RenderableMesh(const DX11MeshID mesh, const IDMap<Mat4>::ItemID localTransformID, const IDMap<Mat4>::ItemID worldTransformID) :
            mMeshID(mesh), mLocalTransformID(localTransformID), mWorldTransformID(worldTransformID)
        {
        }


        DX11MeshID mMeshID;
        IDMap<Mat4>::ItemID mLocalTransformID;
        IDMap<Mat4>::ItemID mWorldTransformID;
    };
    
    typedef std::vector<RenderableMesh> RenderableMeshes;

    struct RenderableMaterial
    {
        RenderableMaterial(const DX11MaterialID diffuseTexture, const DX11MaterialID normalTexture, const float specFactor)
            : mDiffuseTextureID(diffuseTexture), mNormalTextureID(normalTexture), mSpecularFactor(specFactor)
        {
        }

        DX11MaterialID mDiffuseTextureID;
        DX11MaterialID mNormalTextureID;
        float mSpecularFactor;
    };

    struct RenderableModel
    {
        RenderableModel(const DX11MeshID mesh, const IDMap<Mat4>::ItemID localTransformID, const IDMap<Mat4>::ItemID worldTransformID, const DX11MaterialID diffuseTexture, const DX11MaterialID normalTexture, const float specFactor, const float tilingFactor) :
            mMesh(mesh, localTransformID, worldTransformID), mMaterial(diffuseTexture, normalTexture, specFactor), mTextureTilingFactor(tilingFactor)
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
        struct CascadeSplit
        {
            CascadeSplit(const float nearZ, const float farZ, const size_t meshEndIndex) : mNearZ(nearZ), mFarZ(farZ), mMeshEndIndex(meshEndIndex)
            {
            }


            float mNearZ;
            float mFarZ;
            size_t mMeshEndIndex;
        };


        RenderableDirLight(const Vec4& lightColor, const Vec3& lightDir, const uint32_t numCascades) : mLightColor(lightColor), mLightDirection(lightDir), mNumCascades(numCascades)
        {
        }


        Vec4 mLightColor;
        Vec3 mLightDirection;
        uint32_t mNumCascades;

        std::vector<CascadeSplit> mCascadeSplits;
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
        RenderQueue(const IDMap<Mat4>& localTransformStorage, const IDMap<Mat4>& worldTransformStorage);

        void Clear();


        Vec4 mAmbientLight;
        DX11MaterialID mSkyboxTextureID;

        // TODO: more than one camera?
        RenderableCamera mCamera;
        RenderableDirLights mDirectionalLights;
        RenderablePointLights mPointLights;

        const IDMap<Mat4>& mLocalTransformStorage;
        const IDMap<Mat4>& mWorldTransformStorage;
    };
}