#pragma once

#include "include/Core/Types.h"
#include "include/Renderer/IVertexBuffer.h"

#include <vector>

namespace JonsEngine
{
    /* Renderable definition */
    struct Renderable
    {
        Renderable(VertexBufferPtr vertexBuffer, const Mat4& wvpMatrix, const Mat4& worldMatrix, const Vec4& diffuseColor, const float specularFactor) : 
                    mVertexBuffer(vertexBuffer), mTransform(wvpMatrix, worldMatrix), mMaterial(diffuseColor, specularFactor)
        {
        }
        
        /* Transform definition */
        struct Transform
        {
            Transform(const Mat4& wvpMatrix, const Mat4& worldMatrix) : mWVPMatrix(wvpMatrix), mWorldMatrix(worldMatrix)
            {
            }

            Mat4 mWVPMatrix;
            Mat4 mWorldMatrix; 
        };
    
        /* Material definition */
        struct Material
        {
            Material(const Vec4& diffuseColor, const float specularFactor) : mDiffuseColor(diffuseColor), mSpecularFactor(specularFactor)
            {
            }

            Vec4 mDiffuseColor;
            float mSpecularFactor;
        };


        VertexBufferPtr mVertexBuffer;
        Transform mTransform;
        Material mMaterial;
    };
    
    /* RenderQueue definition */
    typedef std::vector<Renderable> RenderQueue;
}