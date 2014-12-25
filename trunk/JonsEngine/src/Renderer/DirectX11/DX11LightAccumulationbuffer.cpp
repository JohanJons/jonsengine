#include "include/Renderer/DirectX11/DX11LightAccumulationbuffer.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Texture.h"


namespace JonsEngine
{
    const float gClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };


    DX11LightAccumulationbuffer::DX11LightAccumulationbuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, D3D11_TEXTURE2D_DESC backbufferTextureDesc) :
        mContext(context), mAccumulationTexture(nullptr), mRTV(nullptr), mSRV(nullptr)
    {
        backbufferTextureDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
        backbufferTextureDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;

        // create acc texture/rtv/srv
        DXCALL(device->CreateTexture2D(&backbufferTextureDesc, NULL, &mAccumulationTexture));
        DXCALL(device->CreateRenderTargetView(mAccumulationTexture, NULL, &mRTV));
        DXCALL(device->CreateShaderResourceView(mAccumulationTexture, NULL, &mSRV))
    }

    DX11LightAccumulationbuffer::~DX11LightAccumulationbuffer()
    {
    }


    void DX11LightAccumulationbuffer::BindForDrawing(ID3D11DepthStencilViewPtr dsv)
    {
        mContext->OMSetRenderTargets(1, &mRTV.p, dsv);
    }

    void DX11LightAccumulationbuffer::ClearAccumulationBuffer()
    {
        mContext->ClearRenderTargetView(mRTV, gClearColor);
    }


    ID3D11ShaderResourceViewPtr DX11LightAccumulationbuffer::GetLightAccumulationBuffer()
    {
        return mSRV;
    }
}
