#pragma once

#include "include/RenderQueue/RenderableContainer.h"
#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
    struct RenderableMaterial : RenderableContainer<RenderableMaterial>
    {
        RenderableMaterial(const DX11TextureID diffuseTexture, const DX11TextureID normalTexture, const float specFactor);


        DX11TextureID mDiffuseTextureID;
        DX11TextureID mNormalTextureID;
        float mSpecularFactor;
    };
}