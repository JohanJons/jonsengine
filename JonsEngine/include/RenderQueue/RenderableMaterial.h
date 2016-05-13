#pragma once

#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
    struct RenderableMaterial
    {
        RenderableMaterial(const DX11MaterialID diffuseTexture, const DX11MaterialID normalTexture, const float specFactor) :
            mDiffuseTextureID(diffuseTexture),
            mNormalTextureID(normalTexture),
            mSpecularFactor(specFactor)
        {
        }

        DX11MaterialID mDiffuseTextureID;
        DX11MaterialID mNormalTextureID;
        float mSpecularFactor;
    };

    typedef std::vector<RenderableMaterial> RenderableMaterials;
}