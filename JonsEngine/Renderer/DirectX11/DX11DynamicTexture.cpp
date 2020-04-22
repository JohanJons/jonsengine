#include "Renderer/DirectX11/DX11DynamicTexture.h"

namespace JonsEngine
{
    DX11DynamicTexture::DX11DynamicTexture(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DXGI_FORMAT textureFormat, uint32_t textureWidth, uint32_t textureHeight, bool createUAV, bool generateMips)
        : mContext( context )
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
		if ( generateMips )
			textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

        DXCALL(device->CreateTexture2D(&textureDesc, nullptr, &mTexture));
        DXCALL(device->CreateRenderTargetView(mTexture, nullptr, &mRTV));
        DXCALL(device->CreateShaderResourceView(mTexture, nullptr, &mSRV));
        if (createUAV)
            DXCALL(device->CreateUnorderedAccessView(mTexture, nullptr, &mUAV));
    }

    void DX11DynamicTexture::BindAsShaderResource(const SHADER_TEXTURE_SLOT shaderTextureSlot) const
    {
        // TODO: rework....
        mContext->VSSetShaderResources( shaderTextureSlot, 1, &mSRV.p );
        mContext->CSSetShaderResources( shaderTextureSlot, 1, &mSRV.p );
        mContext->HSSetShaderResources( shaderTextureSlot, 1, &mSRV.p );
        mContext->DSSetShaderResources( shaderTextureSlot, 1, &mSRV.p );
        mContext->PSSetShaderResources( shaderTextureSlot, 1, &mSRV.p );
    }

    void DX11DynamicTexture::Unbind(const SHADER_TEXTURE_SLOT shaderTextureSlot) const
    {
        mContext->VSSetShaderResources( shaderTextureSlot, 1, &gNullSRV.p );
        mContext->CSSetShaderResources( shaderTextureSlot, 1, &gNullSRV.p );
        mContext->HSSetShaderResources( shaderTextureSlot, 1, &gNullSRV.p );
        mContext->DSSetShaderResources( shaderTextureSlot, 1, &gNullSRV.p );
        mContext->PSSetShaderResources( shaderTextureSlot, 1, &gNullSRV.p );
    }
}