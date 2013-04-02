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
        const Vec3  mLightPosition;
        const float mLightAttenuation;


        // TODO: something other than several vectors...
        std::vector<float> CopyTransformData() const
        {
            std::vector<float> ret;

            std::vector<float> colorValues = GetTypeValues(mColor);
            std::vector<float> wvpMatrixValues = GetTypeValues(mWVP);
            std::vector<float> worldMatrixValues = GetTypeValues(mWorldMatrix);

            ret.insert(ret.end(), colorValues.begin(), colorValues.end());
            ret.insert(ret.end(), wvpMatrixValues.begin(), wvpMatrixValues.end());
            ret.insert(ret.end(), worldMatrixValues.begin(), worldMatrixValues.end());

            return ret;
        }

        std::vector<float> CopyLightData() const
        {
            std::vector<float> ret;

            std::vector<float> ambientLightMatrixValues = GetTypeValues(mAmbientLight);
            std::vector<float> lightIntensityMatrixValues = GetTypeValues(mLightIntensity);
            std::vector<float> lightPositionMatrixValues = GetTypeValues(mLightPosition);

            ret.insert(ret.end(), ambientLightMatrixValues.begin(), ambientLightMatrixValues.end());
            ret.insert(ret.end(), lightIntensityMatrixValues.begin(), lightIntensityMatrixValues.end());
            ret.insert(ret.end(), lightPositionMatrixValues.begin(), lightPositionMatrixValues.end());
            ret.push_back(mLightAttenuation);

            return ret;
        }

        RenderItem(VertexBufferPtr vertexBuffer, const Vec4& color, const Mat4& wvp, const Mat4& worldMatrix, 
                    const Vec4& lightIntensity, const Vec3& lightPosition, const Vec4& ambientLight, const float lightAttenuation);
    };


    /* RenderItem inlines */
    inline RenderItem::RenderItem(VertexBufferPtr vertexBuffer, const Vec4& color, const Mat4& wvp, const Mat4& worldMatrix, 
                                    const Vec4& lightIntensity, const Vec3& lightPosition, const Vec4& ambientLight, const float lightAttenuation) 
                                    : 
                                    mVertexBuffer(vertexBuffer), mColor(color), mWVP(wvp), mWorldMatrix(worldMatrix),
                                    mLightIntensity(lightIntensity), mLightPosition(lightPosition), mAmbientLight(ambientLight), mLightAttenuation(lightAttenuation)
    {
    }
}