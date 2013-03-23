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
        const Mat4 mMVP;
        const Mat4 mMVMatrix;

        const Vec4 mAmbientLight;
        const Vec4 mLightIntensity;
        const Vec3 mDirToLight;

        void CopyUniformData(std::vector<float>& buffer) const
        {
            std::vector<float> colorValues = GetTypeValues(mColor);
            std::vector<float> mvpMatrixValues = GetTypeValues(mMVP);
            std::vector<float> mvMatrixValues = GetTypeValues(mMVMatrix);
            std::vector<float> ambinetLightMatrixValues = GetTypeValues(mAmbientLight);
            std::vector<float> lightIntensityMatrixValues = GetTypeValues(mLightIntensity);
            std::vector<float> dirToLightMatrixValues = GetTypeValues(mDirToLight);

            buffer.insert(buffer.end(), colorValues.begin(), colorValues.end());
            buffer.insert(buffer.end(), mvpMatrixValues.begin(), mvpMatrixValues.end());
            buffer.insert(buffer.end(), mvMatrixValues.begin(), mvMatrixValues.end());
            buffer.insert(buffer.end(), ambinetLightMatrixValues.begin(), ambinetLightMatrixValues.end());
            buffer.insert(buffer.end(), lightIntensityMatrixValues.begin(), lightIntensityMatrixValues.end());
            buffer.insert(buffer.end(), dirToLightMatrixValues.begin(), dirToLightMatrixValues.end());
        }

        RenderItem(VertexBufferPtr vertexBuffer, const Vec4& color, const Mat4& mvp, const Mat4& mv, const Vec4& lightIntensity, const Vec3& dirToLight, const Vec4& ambientLight);
    };


    /* RenderItem inlines */
    inline RenderItem::RenderItem(VertexBufferPtr vertexBuffer, const Vec4& color, const Mat4& mvp, const Mat4& mv, const Vec4& lightIntensity, const Vec3& dirToLight, const Vec4& ambientLight) : 
                                    mVertexBuffer(vertexBuffer), mColor(color), mMVP(mvp), mMVMatrix(mv), mLightIntensity(lightIntensity), mDirToLight(dirToLight), mAmbientLight(ambientLight)
    {
    }
}