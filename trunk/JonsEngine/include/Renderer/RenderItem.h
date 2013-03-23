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

        const Vec4 mAmbientLight;
        const Vec4 mLightIntensity;
        const Vec3 mDirToLight;

        void CopyUniformData(std::vector<float>& buffer) const
        {
            std::vector<float> colorValues = GetTypeValues(mColor);
            std::vector<float> wvpMatrixValues = GetTypeValues(mWVP);
            std::vector<float> worldMatrixValues = GetTypeValues(mWorldMatrix);
            std::vector<float> ambinetLightMatrixValues = GetTypeValues(mAmbientLight);
            std::vector<float> lightIntensityMatrixValues = GetTypeValues(mLightIntensity);
            std::vector<float> dirToLightMatrixValues = GetTypeValues(mDirToLight);

            buffer.insert(buffer.end(), colorValues.begin(), colorValues.end());
            buffer.insert(buffer.end(), wvpMatrixValues.begin(), wvpMatrixValues.end());
            buffer.insert(buffer.end(), worldMatrixValues.begin(), worldMatrixValues.end());
            buffer.insert(buffer.end(), ambinetLightMatrixValues.begin(), ambinetLightMatrixValues.end());
            buffer.insert(buffer.end(), lightIntensityMatrixValues.begin(), lightIntensityMatrixValues.end());
            buffer.insert(buffer.end(), dirToLightMatrixValues.begin(), dirToLightMatrixValues.end());
        }

        RenderItem(VertexBufferPtr vertexBuffer, const Vec4& color, const Mat4& wvp, const Mat4& worldMatrix, const Vec4& lightIntensity, const Vec3& dirToLight, const Vec4& ambientLight);
    };


    /* RenderItem inlines */
    inline RenderItem::RenderItem(VertexBufferPtr vertexBuffer, const Vec4& color, const Mat4& wvp, const Mat4& worldMatrix, const Vec4& lightIntensity, const Vec3& dirToLight, const Vec4& ambientLight) : 
                                    mVertexBuffer(vertexBuffer), mColor(color), mWVP(wvp), mWorldMatrix(worldMatrix), mLightIntensity(lightIntensity), mDirToLight(dirToLight), mAmbientLight(ambientLight)
    {
    }
}