#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Types.h"
#include "include/Core/Platform/Directx11.h"

namespace JonsEngine
{
    class DX11LightAccumulationbuffer
    {
    public:
        DX11LightAccumulationbuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, D3D11_TEXTURE2D_DESC backbufferTextureDesc);
        ~DX11LightAccumulationbuffer();

        void BindAsRenderTarget(ID3D11DepthStencilViewPtr dsv);
        void BindAsShaderResource(const SHADER_TEXTURE_SLOT shaderTextureSlot);
        void Clear();


    private:
        ID3D11DeviceContextPtr mContext;
        ID3D11Texture2DPtr mAccumulationTexture;
        ID3D11ShaderResourceViewPtr mSRV;
        ID3D11RenderTargetViewPtr mRTV;
    };
}
