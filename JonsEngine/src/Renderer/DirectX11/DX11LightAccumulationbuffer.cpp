#include "include/Renderer/DirectX11/DX11LightAccumulationbuffer.h"

#include "include/Renderer/DirectX11/DX11Utils.h"


namespace JonsEngine
{
    DX11LightAccumulationbuffer::DX11LightAccumulationbuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, D3D11_TEXTURE2D_DESC backbufferTextureDesc) :
        mContext(context),
        mAccumulationTexture(nullptr),
        mRTV(nullptr),
        mSRV(nullptr)
    {
        backbufferTextureDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
        backbufferTextureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

        // create acc texture/rtv/srv
        DXCALL(device->CreateTexture2D(&backbufferTextureDesc, NULL, &mAccumulationTexture));
        DXCALL(device->CreateRenderTargetView(mAccumulationTexture, nullptr, &mRTV));
        DXCALL(device->CreateShaderResourceView(mAccumulationTexture, nullptr, &mSRV))
    }

    DX11LightAccumulationbuffer::~DX11LightAccumulationbuffer()
    {
    }


    void DX11LightAccumulationbuffer::BindAsRenderTarget(ID3D11DepthStencilViewPtr dsv)
    {
        mContext->OMSetRenderTargets(1, &mRTV.p, dsv);
    }

    void DX11LightAccumulationbuffer::BindAsShaderResource()
    {
        mContext->PSSetShaderResources(SHADER_TEXTURE_SLOT_DIFFUSE, 1, &mSRV.p);
    }

    void DX11LightAccumulationbuffer::Clear()
    {
        mContext->ClearRenderTargetView(mRTV, GetClearColor());
    }
}