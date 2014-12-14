#include "include/Renderer/DirectX11/DX11LightAccumulationbuffer.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Texture.h"


namespace JonsEngine
{
    const float gClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };


    DX11LightAccumulationbuffer::DX11LightAccumulationbuffer(ID3D11DevicePtr device, D3D11_TEXTURE2D_DESC backbufferTextureDesc) :
        mAccumulationTexture(nullptr), mRTV(nullptr), mSRV(nullptr), mDSV(nullptr), mDSVReadOnly(nullptr), mDepthSRV(nullptr), mDepthStencilState(nullptr)
    {
        backbufferTextureDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

        // create acc texture/rtv/srv
        DXCALL(device->CreateTexture2D(&backbufferTextureDesc, NULL, &mAccumulationTexture));
        DXCALL(device->CreateRenderTargetView(mAccumulationTexture, NULL, &mRTV));
        DXCALL(device->CreateShaderResourceView(mAccumulationTexture, NULL, &mSRV))

        // create depth buffer/view/srv
        D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
        ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
        depthStencilBufferDesc.ArraySize = 1;
        depthStencilBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
        depthStencilBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
        depthStencilBufferDesc.Width = backbufferTextureDesc.Width;
        depthStencilBufferDesc.Height = backbufferTextureDesc.Height;
        depthStencilBufferDesc.MipLevels = 1;
        depthStencilBufferDesc.SampleDesc.Count = 1;
        depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        DXCALL(device->CreateTexture2D(&depthStencilBufferDesc, NULL, &mDepthStencilBuffer));

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        DXCALL(device->CreateShaderResourceView(mDepthStencilBuffer, &srvDesc, &mDepthSRV));

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        DXCALL(device->CreateDepthStencilView(mDepthStencilBuffer, &dsvDesc, &mDSV));

        dsvDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;
        DXCALL(device->CreateDepthStencilView(mDepthStencilBuffer, &dsvDesc, &mDSVReadOnly));

        // depth stencil config used in shading stage
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
        depthStencilDesc.StencilEnable = false;
        DXCALL(device->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState));
    }

    DX11LightAccumulationbuffer::~DX11LightAccumulationbuffer()
    {
    }


    void DX11LightAccumulationbuffer::BindForReading(ID3D11DeviceContextPtr context)
    {
        context->PSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_EXTRA, 1, &mSRV.p);
    }

    void DX11LightAccumulationbuffer::BindForLightingStage(ID3D11DeviceContextPtr context)
    {
        context->OMSetRenderTargets(1, &mRTV.p, mDSVReadOnly);
        context->PSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_DEPTH, 1, &mDepthSRV.p);

        // disable further depth writing
        context->OMSetDepthStencilState(mDepthStencilState, 0);
    }


    void DX11LightAccumulationbuffer::ClearAccumulationBuffer(ID3D11DeviceContextPtr context)
    {
        context->ClearRenderTargetView(mRTV, gClearColor);
    }

    void DX11LightAccumulationbuffer::ClearStencilBuffer(ID3D11DeviceContextPtr context)
    {
        context->ClearDepthStencilView(mDSV, D3D11_CLEAR_STENCIL, 1.0f, 0);
    }


    ID3D11DepthStencilViewPtr DX11LightAccumulationbuffer::GetDepthStencilView()
    {
        return mDSV;
    }
}
