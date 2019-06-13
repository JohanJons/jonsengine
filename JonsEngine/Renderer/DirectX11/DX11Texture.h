#pragma once

#include "Renderer/DirectX11/DX11Utils.h"
#include "Core/Types.h"
#include "Core/Platform.h"

#include <vector>
#include <memory>

namespace JonsEngine
{
    class DX11Texture
    {
    public:
        DX11Texture( ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::vector<uint8_t>& textureData, TextureType textureType, uint32_t textureWidth, uint32_t textureHeight );

        void BindAsShaderResource(const SHADER_TEXTURE_SLOT shaderTextureSlot) const;
        void Unbind( const SHADER_TEXTURE_SLOT shaderTextureSlot ) const;
        void GetDesc( D3D11_TEXTURE2D_DESC& desc )  const;

        const DX11TextureID mID;


    private:
        ID3D11DeviceContextPtr mContext;
		ID3D11Texture2DPtr mTexture = nullptr;
		ID3D11ShaderResourceViewPtr mSRV = nullptr;
    };
}