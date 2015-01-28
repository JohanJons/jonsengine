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
        inline RenderableMaterial(const TextureID diffuseTexture, const TextureID normalTexture, const float specFactor, const float tilingFactor)
            : mDiffuseTextureID(diffuseTexture), mNormalTextureID(normalTexture), mSpecularFactor(specFactor), mTextureTilingFactor(tilingFactor)
        {
        }

        TextureID mDiffuseTextureID;
        TextureID mNormalTextureID;
        float mSpecularFactor;
        float mTextureTilingFactor;
    };

    struct RenderableModel
    {
        inline RenderableModel(const MeshID mesh, const Mat4& wvpMatrix, const Mat4& worldMatrix, const TextureID diffuseTexture, const TextureID normalTexture, const float specFactor, const float tilingFactor) :
            mMesh(mesh, wvpMatrix, worldMatrix), mMaterial(diffuseTexture, normalTexture, specFactor, tilingFactor)
        {
        }

        RenderableMesh mMesh;
        RenderableMaterial mMaterial;
    };

    typedef std::vector<RenderableModel> RenderableModels;

    struct RenderableCamera
    {
		Mat4 mCameraViewMatrix;
        Mat4 mCameraProjectionMatrix;
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
        inline RenderablePointLight(const Mat4& wvpMatrix, const Vec4& lightColor, const Vec3& lightPos, const float lightIntensity, const float maxDistance) :
            mWVPMatrix(wvpMatrix), mLightColor(lightColor), mLightPosition(lightPos), mLightIntensity(lightIntensity), mMaxDistance(maxDistance)
        {
        }


		enum POINT_LIGHT_DIR
		{
			POINT_LIGHT_DIR_POS_X = 0,
			POINT_LIGHT_DIR_NEG_X,
			POINT_LIGHT_DIR_POS_Y,
			POINT_LIGHT_DIR_NEG_Y,
			POINT_LIGHT_DIR_NEG_Z,
			POINT_LIGHT_DIR_POS_Z,
			POINT_LIGHT_DIR_COUNT
		};


        Mat4 mWVPMatrix;
        Vec4 mLightColor;
        Vec3 mLightPosition;

        float mLightIntensity;
        float mMaxDistance;

		std::array<RenderableMeshes, POINT_LIGHT_DIR::POINT_LIGHT_DIR_COUNT> mMeshes;
        std::array<Mat4, POINT_LIGHT_DIR::POINT_LIGHT_DIR_COUNT> mFaceWVPMatrices;
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