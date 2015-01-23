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

    struct RenderableCamera
    {
		Mat4 mCameraViewMatrix;
        Mat4 mCameraProjectionMatrix;
		Vec3 mCameraPosition;

        std::vector<RenderableMesh> mMesh;
        RenderableMaterial mMaterial;
    };

    struct RenderableDirLight
    {
        Vec4 mLightColor;
        Vec3 mLightDirection;

        RenderableMeshes mMeshes;
    };
    
    typedef std::vector<RenderableDirLight> RenderableDirLights;

    struct RenderablePointLight
    {
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
        Mat4 mWorldMatrix;
        Vec4 mLightColor;
        Vec3 mLightPosition;

        float mLightIntensity;
        float mMaxDistance;

		std::array<RenderableMeshes, POINT_LIGHT_DIR::POINT_LIGHT_DIR_COUNT> mMeshes;
    };
    
    typedef std::vector<RenderablePointLight> RenderablePointLights;

    struct RenderQueue
    {
        void Clear();


        Vec4 mAmbientLight;
        float mFOV;

        // TODO: more than one camera?
        RenderableCamera mCamera;
        RenderableDirLights mDirectionalLights;
        RenderablePointLights mPointLights;
    };
}