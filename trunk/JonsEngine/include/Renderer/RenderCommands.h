#pragma once

#include "include/Core/Types.h"

#include <array>

namespace JonsEngine
{
    struct Renderable
    {
        Renderable(const MeshID mesh, const Mat4& wvpMatrix, const Mat4& worldMatrix, const float textureTilingFactor, const bool lightingEnabled, const float specularFactor)
            :
                    Renderable(mesh, wvpMatrix, worldMatrix, textureTilingFactor, lightingEnabled, specularFactor, INVALID_TEXTURE_ID, Vec4(1.0f), Vec4(1.0f), Vec4(1.0f), Vec4(1.0f))
        {

        }
            
        Renderable(const MeshID mesh, const Mat4& wvpMatrix, const Mat4& worldMatrix, const float textureTilingFactor, const bool lightingEnabled, const float specularFactor,
                    const TextureID texture, Vec4 diffuseColor, Vec4 ambientColor, Vec4 specularColor, Vec4 emissiveColor)
            :
                    mMesh(mesh), mWVPMatrix(wvpMatrix), mWorldMatrix(worldMatrix), mTextureTilingFactor(textureTilingFactor), mLightingEnabled(lightingEnabled), mSpecularFactor(specularFactor),
                    mDiffuseTexture(texture), mDiffuseColor(diffuseColor), mAmbientColor(ambientColor), mSpecularColor(specularColor), mEmissiveColor(emissiveColor)
        {

        }

        MeshID mMesh;
        Mat4 mWVPMatrix;
        Mat4 mWorldMatrix; 
        float mTextureTilingFactor;

        TextureID mDiffuseTexture;
        Vec4 mDiffuseColor;
        Vec4 mAmbientColor;
        Vec4 mSpecularColor;
        Vec4 mEmissiveColor;
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