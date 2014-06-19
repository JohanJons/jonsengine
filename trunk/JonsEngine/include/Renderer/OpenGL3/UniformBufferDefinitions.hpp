#include "include/Core/Types.h"

#include <array>

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
        uint32_t mSamples;


        UnifPointLight(const Mat4& vwpMatrix, const Vec4& lightColor, const Vec4& lightPos, const Vec4& gamma, const Vec2& screenSize, const float lightIntensity, const float maxDistance, const uint32_t samples)
            :
            mWVPMatrix(vwpMatrix), mLightColor(lightColor), mLightPos(lightPos), mGamma(gamma), mScreenSize(screenSize), mLightIntensity(lightIntensity), mMaxDistance(maxDistance), mSamples(samples)
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
        std::array<Mat4, 4> mLightMatrices;
        Mat4 mCameraViewMatrix;
        Vec4 mSplitDistances;
        Vec4 mLightColor;
        Vec4 mLightDir;
        Vec4 mGamma;
        Vec2 mScreenSize;
        uint32_t mSamples;


        UnifDirLight(const std::array<Mat4, 4>& lightMatrices, const Mat4& cameraViewMatrix, const std::array<float, 4>& splitDistances, const Vec4& lightColor, const Vec4& lightDir, const Vec4& gamma, const Vec2& screenSize, const uint32_t samples)
            :
            mLightMatrices(lightMatrices), mCameraViewMatrix(cameraViewMatrix), mSplitDistances(splitDistances[0], splitDistances[1], splitDistances[2], splitDistances[3]), mLightColor(lightColor), mLightDir(lightDir), mGamma(gamma), mScreenSize(screenSize), mSamples(samples)
        {
        }
    };

    struct UnifAmbientLight
    {
        Vec4 mLightColor;
        Vec4 mGamma;
        Vec2 mScreenSize;
        uint32_t mSamples;


        UnifAmbientLight(const Vec4& lightColor, const Vec4& gamma, const Vec2& screenSize, const uint32_t samples)
            :
            mLightColor(lightColor), mGamma(gamma), mScreenSize(screenSize), mSamples(samples)
        {
        }
    };

    struct UnifDepth
    {
        Vec2 mScreenSize;
        float mZNear;
        float mZFar;


        UnifDepth(const Vec2& screenSize, const float zNear, const float zFar)
            :
        mScreenSize(screenSize), mZNear(zNear), mZFar(zFar)
        {
        }
    };
}