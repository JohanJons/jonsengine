#include "Renderer/DirectX11/DX11RenderTarget2D.h"

namespace JonsEngine
{
    DX11RenderTarget2D::DX11RenderTarget2D(ID3D11DevicePtr device, const DXGI_FORMAT textureFormat, const uint32_t textureWidth, const uint32_t textureHeight, const bool createUAV)
    {
        D3D11_TEXTURE2D_DESC textureDesc;
        ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
        textureDesc.Width = textureWidth;
        textureDesc.Height = textureHeight;
        textureDesc.ArraySize = 1;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Format = textureFormat;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        if (createUAV)
            textureDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

        DXCALL(device->CreateTexture2D(&textureDesc, nullptr, &mTexture));
        DXCALL(device->CreateRenderTargetView(mTexture, nullptr, &mRTV));
        DXCALL(device->CreateShaderResourceView(mTexture, nullptr, &mSRV));
        if (createUAV)
            DXCALL(device->CreateUnorderedAccessView(mTexture, nullptr, &mUAV));
    }
    
    DX11RenderTarget2D::~DX11RenderTarget2D()
    {
    }
}