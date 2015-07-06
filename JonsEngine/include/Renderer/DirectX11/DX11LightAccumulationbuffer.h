#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Core/Types.h"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11LightAccumulationbuffer
    {
    public:
        DX11LightAccumulationbuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const uint32_t textureWidth, const uint32_t textureHeight);
        ~DX11LightAccumulationbuffer();

        void BindAsRenderTarget(ID3D11DepthStencilViewPtr dsv);
        void BindAsShaderResource(const DX11Texture::SHADER_TEXTURE_SLOT shaderTextureSlot);
        void Clear();


    private:
        ID3D11DeviceContextPtr mContext;
        DX11Texture mTexture;
        ID3D11ShaderResourceViewPtr mSRV;
        ID3D11RenderTargetViewPtr mRTV;
    };
}
