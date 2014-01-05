#include "include/Core/Types.h"

namespace JonsEngine
{
    struct UnifGeometry
    {
        Mat4 mWVPMatrix;
        Mat4 mWorldMatrix;
        uint32_t mHasDiffuseTexture;
        uint32_t mHasNormalTexture;
        float mTextureTilingFactor;


        UnifGeometry(const Mat4& WVPMatrix, const Mat4& WorldMatrix, const uint32_t hasDiffuseTexture, const uint32_t hasNormalTexture, const float textureTilingFactor)
            :
            mWVPMatrix(WVPMatrix), mWorldMatrix(WorldMatrix), mHasDiffuseTexture(hasDiffuseTexture), mHasNormalTexture(hasNormalTexture), mTextureTilingFactor(textureTilingFactor)
        {
        }
    };

    struct UnifPointLight
    {
        Mat4 mWVPMatrix;
        Vec4 mLightColor;
        Vec4 mLightPos;
        Vec4 mGamma;
        Vec2 mScreenSize;
        float mLightIntensity;
        float mMaxDistance;


        UnifPointLight(const Mat4& vwpMatrix, const Vec4& lightColor, const Vec4& lightPos, const Vec4& gamma, const Vec2& screenSize, const float lightIntensity, const float maxDistance)
            :
            mWVPMatrix(vwpMatrix), mLightColor(lightColor), mLightPos(lightPos), mGamma(gamma), mScreenSize(screenSize), mLightIntensity(lightIntensity), mMaxDistance(maxDistance)
        {
        }
    };

    struct UnifNull
    {
        Mat4 mWVPMatrix;


        UnifNull(const Mat4& wvpMatrix)
            :
            mWVPMatrix(wvpMatrix)
        {
        }
    };

    struct UnifDirLight
    {
        Mat4 mWVPMatrix;
        Vec4 mLightColor;
        Vec4 mLightDir;
        Vec4 mGamma;
        Vec2 mScreenSize;


        UnifDirLight(const Mat4& vwpMatrix, const Vec4& lightColor, const Vec4& lightDir, const Vec4& gamma, const Vec2& screenSize)
            :
            mWVPMatrix(vwpMatrix), mLightColor(lightColor), mLightDir(lightDir), mGamma(gamma), mScreenSize(screenSize)
        {
        }
    };

    struct UnifAmbientLight
    {
        Mat4 mWVPMatrix;
        Vec4 mLightColor;
        Vec4 mGamma;
        Vec2 mScreenSize;


        UnifAmbientLight(const Mat4& wvpMatrix, const Vec4& lightColor, const Vec4& gamma, const Vec2& screenSize)
            :
            mWVPMatrix(wvpMatrix), mLightColor(lightColor), mGamma(gamma), mScreenSize(screenSize)
        {
        }
    };

    struct UnifDepth
    {
        Mat4 mWVPMatrix;
        Vec2 mScreenSize;
        float mZNear;
        float mZFar;


        UnifDepth(const Mat4& wvpMatrix, const Vec2& screenSize, const float zNear, const float zFar)
            :
            mWVPMatrix(wvpMatrix), mScreenSize(screenSize), mZNear(zNear), mZFar(zFar)
        {
        }
    };
}