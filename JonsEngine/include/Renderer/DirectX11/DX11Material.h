#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Core/Types.h"

#include <d3d11.h>
#include <vector>
#include <memory>

namespace JonsEngine
{
    class DX11Material
    {
    public:
        DX11Material(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const DXGI_FORMAT textureFormat, const uint32_t textureWidth, const uint32_t textureHeight,
            const uint32_t numTextures, const DX11Texture::TextureDimension dimension, const std::vector<uint8_t>& textureData, const bool genMipmaps);
        ~DX11Material();

        void BindAsShaderResource(const DX11Texture::SHADER_TEXTURE_SLOT shaderTextureSlot);

        const MaterialID mTextureID;


    private:
        ID3D11DeviceContextPtr mContext;
        DX11Texture mTexture;
        ID3D11ShaderResourceViewPtr mSRV;
    };
}