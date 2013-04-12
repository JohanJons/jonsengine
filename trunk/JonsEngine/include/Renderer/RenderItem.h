#pragma once

#include "include/Core/Types.h"
#include "include/Renderer/IVertexBuffer.h"

namespace JonsEngine
{

    /* RenderItem definition */
    struct RenderItem
    {
        VertexBufferPtr mVertexBuffer;
        const Vec4 mColor;
        const Mat4 mWVP;
        const Mat4 mWorldMatrix;

        const Vec4  mAmbientLight;
        const Vec4  mLightIntensity;
        const Vec4  mGamma;
        const Vec3  mLightPosition;
        const Vec3  mViewDirection;
        const float mLightAttenuation;
        const float mShininessFactor;


        std::vector<float> CopyTransformData() const
        {
            std::vector<float> ret;

            GetTypeValues(mColor, ret);
            GetTypeValues(mWVP, ret);
            GetTypeValues(mWorldMatrix, ret);

            return ret;
        }

        std::vector<float> CopyLightData() const
        {
            std::vector<float> ret;

            GetTypeValues(mAmbientLight, ret);
            GetTypeValues(mLightIntensity, ret);
            GetTypeValues(mGamma, ret);
            GetTypeValues(mLightPosition, ret);
            ret.push_back(0.0f);                    // TODO: find better way to automate padding
            GetTypeValues(mViewDirection, ret);
            ret.push_back(mLightAttenuation);
            ret.push_back(mShininessFactor);

            return ret;
        }

        RenderItem(VertexBufferPtr vertexBuffer, const Vec4& color, const Mat4& wvp, const Mat4& worldMatrix, const Vec4& lightIntensity, const Vec4& gamma, 
                   const Vec3& lightPosition, const Vec4& ambientLight, const Vec3 viewDirection, const float lightAttenuation, const float shininessFactor);
    };


    /* RenderItem inlines */
    inline RenderItem::RenderItem(VertexBufferPtr vertexBuffer, const Vec4& color, const Mat4& wvp, const Mat4& worldMatrix, const Vec4& lightIntensity, const Vec4& gamma,
                                    const Vec3& lightPosition, const Vec4& ambientLight, const Vec3 viewDirection, const float lightAttenuation, const float shininessFactor) 
                                    : 
                                    mVertexBuffer(vertexBuffer), mColor(color), mWVP(wvp), mWorldMatrix(worldMatrix), mLightIntensity(lightIntensity), mGamma(gamma),
                                        mLightPosition(lightPosition), mAmbientLight(ambientLight), mViewDirection(viewDirection), mLightAttenuation(lightAttenuation), mShininessFactor(shininessFactor)
    {
    }
}