#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/Shaders/Constants.h"
#include "include/Core/Types.h"

#include <d3d11.h>
#include <vector>
#include <memory>

namespace JonsEngine
{
    class DX11Texture
    {
    public:
        enum SHADER_TEXTURE_SLOT
        {
            SHADER_TEXTURE_SLOT_DIFFUSE = TEXTURE_SLOT_DIFFUSE,
            SHADER_TEXTURE_SLOT_NORMAL = TEXTURE_SLOT_NORMAL,
            SHADER_TEXTURE_SLOT_DEPTH = TEXTURE_SLOT_DEPTH,
            SHADER_TEXTURE_SLOT_EXTRA = TEXTURE_SLOT_EXTRA,
            NUM_SHADER_TEXTURE_SLOTS
        };

        enum class TextureDimension
        {
            Texture2D,
            Texture2DArray,
            TextureCube
        };

        DX11Texture(IDXGISwapChainPtr swapchain);
        DX11Texture(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const DXGI_FORMAT textureFormat, const uint32_t textureWidth, const uint32_t textureHeight,
            const uint32_t numTextures, const TextureDimension dimension, const bool isRenderTarget, const bool isDepthTexture);
        DX11Texture(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const DXGI_FORMAT textureFormat, const uint32_t textureWidth, const uint32_t textureHeight,
            const uint32_t numTextures, const TextureDimension dimension, const bool isRenderTarget, const bool isDepthTexture, const std::vector<uint8_t>& textureData, const bool genMipmaps);
        ~DX11Texture();

        ID3D11ShaderResourceViewPtr CreateSRV(ID3D11DevicePtr device);
        ID3D11ShaderResourceViewPtr CreateSRV(ID3D11DevicePtr device, const DXGI_FORMAT srvFormat);
        ID3D11RenderTargetViewPtr CreateRTV(ID3D11DevicePtr device);
        ID3D11RenderTargetViewPtr CreateRTV(ID3D11DevicePtr device, const DXGI_FORMAT rtvFormat);
        ID3D11UnorderedAccessViewPtr CreateUAV(ID3D11DevicePtr device);
        ID3D11DepthStencilViewPtr CreateDSV(ID3D11DevicePtr device);


        const TextureDimension mTextureDimension;


    private:
        ID3D11DeviceContextPtr mContext;
        ID3D11Texture2DPtr mTexture;
    };
}
