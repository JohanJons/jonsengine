#include "include/Renderer/DirectX11/DX11LightAccumulationbuffer.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Texture.h"


namespace JonsEngine
{
    static const float gClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };


    DX11LightAccumulationbuffer::DX11LightAccumulationbuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const uint32_t textureWidth, const uint32_t textureHeight) :
        mContext(context),
        mTexture(device, context, DXGI_FORMAT_R16G16B16A16_UNORM, textureWidth, textureHeight, 1, DX11Texture::TextureDimension::Texture2D, true, false),
        mRTV(mTexture.CreateRTV(device)),
        mSRV(mTexture.CreateSRV(device))
    {
    }

    DX11LightAccumulationbuffer::~DX11LightAccumulationbuffer()
    {
    }


    void DX11LightAccumulationbuffer::BindAsRenderTarget(ID3D11DepthStencilViewPtr dsv)
    {
        mContext->OMSetRenderTargets(1, &mRTV.p, dsv);
    }

    void DX11LightAccumulationbuffer::BindAsShaderResource(const DX11Texture::SHADER_TEXTURE_SLOT shaderTextureSlot)
    {
        mContext->PSSetShaderResources(shaderTextureSlot, 1, &mSRV.p);
    }

    void DX11LightAccumulationbuffer::Clear()
    {
        mContext->ClearRenderTargetView(mRTV, gClearColor);
    }
}
