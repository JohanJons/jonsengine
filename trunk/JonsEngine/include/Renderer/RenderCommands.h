#pragma once

#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
    struct Renderable
    {
        MeshID mMesh;
        Mat4 mWVPMatrix;
        Mat4 mWorldMatrix;
        float mTextureTilingFactor;

        Vec4 mDiffuseColor;
        Vec4 mAmbientColor;
        Vec4 mSpecularColor;
        Vec4 mEmissiveColor;

        TextureID mDiffuseTexture;
        TextureID mNormalTexture;
        bool mLightingEnabled;
        float mSpecularFactor;


        Renderable(const MeshID mesh, const Mat4& wvpMatrix, const Mat4& worldMatrix, const bool lightingEnabled)
            :
            Renderable(mesh, wvpMatrix, worldMatrix, 1.0f, Vec4(1.0f), Vec4(1.0f), Vec4(1.0f), Vec4(0.0f), INVALID_TEXTURE_ID, INVALID_TEXTURE_ID, lightingEnabled, 0.02f)
        {

        }

        Renderable(const MeshID mesh, const Mat4& wvpMatrix, const Mat4& worldMatrix, const float textureTilingFactor,
            const Vec4& diffuseColor, const Vec4& ambientColor, const Vec4& specularColor, const Vec4& emissiveColor,
            const TextureID diffuseTexture, const TextureID normalTexture, const bool lightingEnabled, const float specularFactor)
            :
            mMesh(mesh), mWVPMatrix(wvpMatrix), mWorldMatrix(worldMatrix), mTextureTilingFactor(textureTilingFactor),
            mDiffuseColor(diffuseColor), mAmbientColor(ambientColor), mSpecularColor(specularColor), mEmissiveColor(emissiveColor),
            mDiffuseTexture(diffuseTexture), mNormalTexture(normalTexture), mLightingEnabled(lightingEnabled), mSpecularFactor(specularFactor)
        {

        }
    };

    typedef std::vector<Renderable> RenderQueue;


    struct RenderableLighting
    {
        struct PointLight
        {
            Mat4 mWVPMatrix;
            Mat4 mWorldMatrix;
            Vec4 mLightColor;
            Vec3 mLightPosition;

            float mLightIntensity;
            float mMaxDistance;


            PointLight(const Mat4& wvpMatrix, const Mat4& worldMatrix, const Vec4& lightColor, const Vec3& lightPosition, const float intensity, const float maxDistance) :
                mWVPMatrix(wvpMatrix), mWorldMatrix(worldMatrix), mLightColor(lightColor), mLightPosition(lightPosition), mLightIntensity(intensity), mMaxDistance(maxDistance)
            {
            }
        };

        struct DirectionalLight
        {
            Vec4 mLightColor;
            Vec3 mLightDirection;


            DirectionalLight(const Vec4& lightColor, const Vec3& lightDirection) : mLightColor(lightColor), mLightDirection(lightDirection)
            {
            }
        };

        Mat4 mCameraViewMatrix;
        Mat4 mCameraProjectionMatrix;
        Vec4 mGamma;
        Vec4 mAmbientLight;
        Vec3 mCameraPosition;
        Vec3 mCameraDirection;
        Vec2 mScreenSize;
       
        std::vector<PointLight> mPointLights;
        std::vector<DirectionalLight> mDirectionalLights;


        RenderableLighting(const Mat4& camViewMatrix,  const Mat4& camProjMatrix, const Vec4& gamma, const Vec4& ambientLight, const Vec3& cameraPosition, const Vec3& cameraDirection, const Vec2& screenSize) :
            mCameraViewMatrix(camViewMatrix), mCameraProjectionMatrix(camProjMatrix), mGamma(gamma), mScreenSize(screenSize), mCameraPosition(cameraPosition), mCameraDirection(cameraDirection), mAmbientLight(ambientLight)
        {
        }
    };
}