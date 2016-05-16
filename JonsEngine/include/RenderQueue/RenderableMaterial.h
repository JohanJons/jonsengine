#pragma once

#include "include/RenderQueue/RenderableCollection.h"
#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
    struct RenderableMaterial : RenderableCollection<RenderableMaterial>
    {
        RenderableMaterial(const DX11MaterialID diffuseTexture, const DX11MaterialID normalTexture, const float specFactor);


        DX11MaterialID mDiffuseTextureID;
        DX11MaterialID mNormalTextureID;
        float mSpecularFactor;
    };
}