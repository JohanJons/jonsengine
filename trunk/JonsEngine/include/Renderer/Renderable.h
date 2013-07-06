#pragma once

#include "include/Core/Types.h"
#include "include/Renderer/IMesh.h"

#include "GL/glew.h"
#include <vector>

namespace JonsEngine
{
    /* Renderable definition */
    struct Renderable
    {
        Renderable(const MeshPtr mesh, const Mat4& wvpMatrix, const Mat4& worldMatrix, const bool lightingEnabled, const float specularFactor)
                   :
            mMesh(mesh), mWVPMatrix(wvpMatrix), mWorldMatrix(worldMatrix), mLightingEnabled(lightingEnabled), mSpecularFactor(specularFactor)
        {
        }

        MeshPtr mMesh;
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