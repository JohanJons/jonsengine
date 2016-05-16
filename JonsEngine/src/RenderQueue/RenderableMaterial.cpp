#include "include/RenderQueue/RenderableMaterial.h"

namespace JonsEngine
{
    RenderableMaterial::RenderableMaterial(const DX11MaterialID diffuseTexture, const DX11MaterialID normalTexture, const float specFactor) :
        mDiffuseTextureID(diffuseTexture),
        mNormalTextureID(normalTexture),
        mSpecularFactor(specFactor)
    {
    }
}