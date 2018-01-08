#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Types.h"
#include "include/Core/Platform/Directx11.h"

#include <vector>
#include <memory>

namespace JonsEngine
{
    class DX11Texture
    {
    public:
        DX11Texture(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::vector<uint8_t>& textureData, const DXGI_FORMAT textureFormat, const uint32_t textureWidth, const uint32_t textureHeight, const bool isCubeTexture);
        ~DX11Texture();

        void BindAsShaderResource(const SHADER_TEXTURE_SLOT shaderTextureSlot) const;

        const DX11TextureID mID;


    private:
        ID3D11DeviceContextPtr mContext;
        ID3D11Texture2DPtr mTexture;
        ID3D11ShaderResourceViewPtr mSRV;
    };
}