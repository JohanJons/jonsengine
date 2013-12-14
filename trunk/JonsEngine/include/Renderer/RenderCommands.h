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
            Vec4 mLightColor;
            Vec4 mLightPosition;

            float mFalloffFactor;
            float mMaxDistance;


            PointLight(const Mat4& wvpMatrix, const Vec4& lightColor, const Vec4& lightPosition, const float fallOff, const float maxDistance) :
                mWVPMatrix(wvpMatrix), mLightColor(lightColor), mLightPosition(lightPosition), mFalloffFactor(fallOff), mMaxDistance(maxDistance)
            {
            }
        };

        struct DirectionalLight
        {
            Vec4 mLightColor;
            Vec4 mLightDirection;


            DirectionalLight(const Vec4& lightColor, const Vec4& lightDirection) : mLightColor(lightColor), mLightDirection(lightDirection)
            {
            }
        };

        Vec4 mGamma;
        Vec2 mScreenSize;
        Vec4 mAmbientLight;
       
        std::vector<PointLight> mPointLights;
        std::vector<DirectionalLight> mDirectionalLights;


        RenderableLighting(const Vec4& gamma, const Vec2& screenSize, const Vec4& ambientLight) : mGamma(gamma), mScreenSize(screenSize), mAmbientLight(ambientLight)
        {
        }
    };
}