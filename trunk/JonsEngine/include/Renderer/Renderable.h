#pragma once

#include "include/Core/Types.h"
#include "include/Renderer/IVertexBuffer.h"

#include "GL/glew.h"
#include <vector>

namespace JonsEngine
{
    /* Renderable definition */
    struct Renderable
    {
        Renderable(const VertexBufferPtr vertexBuffer, const Mat4& wvpMatrix, const Mat4& worldMatrix, const TexturePtr diffuseTexture, const Vec4& diffuseColor, const Vec4& ambientColor, const Vec4& specularColor,
                   const Vec4& emissiveColor, const bool lightingEnabled, const float specularFactor)
                   :
            mVertexBuffer(vertexBuffer), mWVPMatrix(wvpMatrix), mWorldMatrix(worldMatrix), mDiffuseTexture(diffuseTexture), mDiffuseColor(diffuseColor), mAmbientColor(ambientColor), mSpecularColor(specularColor), 
            mEmissiveColor(emissiveColor), mLightingEnabled(lightingEnabled), mSpecularFactor(specularFactor)
        {
        }

        VertexBufferPtr mVertexBuffer;
        Mat4 mWVPMatrix;
        Mat4 mWorldMatrix; 

        TexturePtr mDiffuseTexture;
        Vec4 mDiffuseColor;
        Vec4 mAmbientColor;
        Vec4 mSpecularColor;
        Vec4 mEmissiveColor;
        bool mLightingEnabled;
        float mSpecularFactor;
    };
    
    /* RenderQueue definition */
    typedef std::vector<Renderable> RenderQueue;
}