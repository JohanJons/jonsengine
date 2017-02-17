#include "include/RenderQueue/RenderableMaterial.h"

namespace JonsEngine
{
    RenderableMaterial::RenderableMaterial(const DX11TextureID diffuseTexture, const DX11TextureID normalTexture, const float specFactor) :
        mDiffuseTextureID(diffuseTexture),
        mNormalTextureID(normalTexture),
        mSpecularFactor(specFactor)
    {
    }
}