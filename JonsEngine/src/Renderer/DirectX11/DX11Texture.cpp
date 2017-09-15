#include "include/Renderer/DirectX11/DX11Texture.h"

namespace JonsEngine
{
    static DX11TextureID gNextTextureID = 1;

    uint32_t GetNumMipLevels(uint32_t width, uint32_t height);


    DX11Texture::DX11Texture(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::vector<uint8_t>& textureData, const DXGI_FORMAT textureFormat, const uint32_t textureWidth, const uint32_t textureHeight, const bool isCubeTexture) :
        mID(gNextTextureID++),
        mContext(context),
        mTexture(nullptr),
        mSRV(nullptr)
    {
        D3D11_TEXTURE2D_DESC textureDesc;
        ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
        textureDesc.Width = textureWidth;
        textureDesc.Height = textureHeight;
        textureDesc.ArraySize = isCubeTexture ? 6 : 1;
        textureDesc.Format = textureFormat;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.MipLevels = 0;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
        if (isCubeTexture)
            textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
        DXCALL(device->CreateTexture2D(&textureDesc, NULL, &mTexture));

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        srvDesc.Format = textureDesc.Format;
        srvDesc.ViewDimension = isCubeTexture ? D3D11_SRV_DIMENSION_TEXTURECUBE : D3D11_SRV_DIMENSION_TEXTURE2D;
        if (isCubeTexture)
            srvDesc.TextureCube.MipLevels = -1;
        else
            srvDesc.Texture2D.MipLevels = -1;
        DXCALL(device->CreateShaderResourceView(mTexture, &srvDesc, &mSRV));

        // mip-level 0 data
        uint32_t sizeWidth = textureWidth * sizeof(uint8_t) * 4;
        if (isCubeTexture)
        {
            for (uint32_t index = 0; index < 6; ++index)
            {
                const uint32_t subResourceID = D3D11CalcSubresource(0, index, GetNumMipLevels(textureWidth, textureHeight));
                context->UpdateSubresource(mTexture, subResourceID, NULL, &textureData.at(sizeWidth * textureHeight * index), sizeWidth, 0);
            }
        }
        else
            context->UpdateSubresource(mTexture, 0, NULL, &textureData.at(0), sizeWidth, 0);

        context->GenerateMips(mSRV);
    }

    DX11Texture::~DX11Texture()
    {
    }


    void DX11Texture::BindAsShaderResource(const SHADER_TEXTURE_SLOT shaderTextureSlot)
    {
		mContext->VSSetShaderResources(shaderTextureSlot, 1, &mSRV.p);
        mContext->PSSetShaderResources(shaderTextureSlot, 1, &mSRV.p);
    }


    uint32_t GetNumMipLevels(uint32_t width, uint32_t height)
    {
        uint32_t numLevels = 1;
        const uint32_t minLevel = 1;
        while ((width > 1) || (height > 1))
        {
            width = std::max(width / 2, minLevel);
            height = std::max(height / 2, minLevel);
            ++numLevels;
        }

        return numLevels;
    }
}
