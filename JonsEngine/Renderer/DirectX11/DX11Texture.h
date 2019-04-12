#pragma once

#include "DX11Utils.h"
#include "Types.h"
#include "Platform.h"

#include <vector>
#include <memory>

namespace JonsEngine
{
    class DX11Texture
    {
    public:
        DX11Texture( ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::vector<uint8_t>& textureData, const TextureType textureType, const uint32_t textureWidth, const uint32_t textureHeight );

        void BindAsShaderResource(const SHADER_TEXTURE_SLOT shaderTextureSlot) const;

        const DX11TextureID mID;


    private:
        ID3D11DeviceContextPtr mContext;
		ID3D11Texture2DPtr mTexture = nullptr;
		ID3D11ShaderResourceViewPtr mSRV = nullptr;
    };
}