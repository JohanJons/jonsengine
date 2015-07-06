#include "include/Renderer/DirectX11/DX11Material.h"

namespace JonsEngine
{
    static MaterialID gNextTextureID = 1;


    DX11Material::DX11Material(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const DXGI_FORMAT textureFormat, const uint32_t textureWidth, const uint32_t textureHeight, const uint32_t numTextures, const DX11Texture::TextureDimension dimension, const std::vector<uint8_t>& textureData, const bool genMipmaps) :
        mTextureID(gNextTextureID++),
        mContext(context),
        mTexture(device, context, textureFormat, textureWidth, textureHeight, numTextures, dimension, false, false, textureData, genMipmaps),
        mSRV(mTexture.CreateSRV(device))
    {
    }

    DX11Material::~DX11Material()
    {
    }


    void DX11Material::BindAsShaderResource(const DX11Texture::SHADER_TEXTURE_SLOT shaderTextureSlot)
    {
        mContext->PSSetShaderResources(shaderTextureSlot, 1, &mSRV.p);
    }
}
