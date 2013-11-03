#pragma once

#include "include/Core/Types.h"

#include <array>

namespace JonsEngine
{
    struct Renderable
    {
        Renderable(const MeshID mesh, const Mat4& wvpMatrix, const Mat4& worldMatrix, const bool lightingEnabled)
            :
                    Renderable(mesh, wvpMatrix, worldMatrix, 1.0f,
                               Vec4(1.0f), Vec4(1.0f), Vec4(1.0f), Vec4(0.0f),
                               INVALID_TEXTURE_ID, INVALID_TEXTURE_ID, lightingEnabled, 0.02f)
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
    };

    typedef std::vector<Renderable> RenderQueue;


    struct RenderableLighting
    {
        RenderableLighting(const Vec4& gamma, const Vec3& viewDirection, const uint32_t usedLights) : mGamma(gamma), mViewDirection(viewDirection), mUsedLights(usedLights)
        {
        }

        /* Light definition */
        struct Light
        {
            Light() : mLightColor(1.0f), mLightPosition(0.0f), mLightDirection(0.0f), mLightType(0), mIntensity(1.0f), mRadius(0.0f), mMaxDistance(0.0f)
            {
            }

            Vec4 mLightColor;
            Vec4 mLightPosition;
            Vec4 mLightDirection;

            uint32_t mLightType;
            float mIntensity;
            float mRadius;
            float mMaxDistance;
        };

        /* Lights definition */
        typedef std::array<Light, 8> Lights;

        Vec4 mGamma;
        Vec3 mViewDirection;
        uint32_t mUsedLights;
       
        Lights mLights;
    };
}