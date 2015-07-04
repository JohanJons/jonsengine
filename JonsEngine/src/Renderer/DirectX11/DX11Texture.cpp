#include "include/Renderer/DirectX11/DX11Texture.h"

#include "include/Renderer/DirectX11/DX11Utils.h"

namespace JonsEngine
{
    static TextureID gNextTextureID = 1;

    uint32_t GetNumMipLevels(uint32_t width, uint32_t height);


    DX11Texture::DX11Texture(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const DXGI_FORMAT textureFormat, const uint32_t textureWidth, const uint32_t textureHeight,
        const SHADER_TEXTURE_SLOT textureSlot, const uint32_t numTextures, const bool isCubeTexture, const bool isDepthTexture) :
        DX11Texture(device, context, std::vector<uint8_t>(), textureFormat, textureWidth, textureHeight, textureSlot, numTextures, isCubeTexture, isDepthTexture, false)
    {
    }

    DX11Texture::DX11Texture(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::vector<uint8_t>& textureData, const DXGI_FORMAT textureFormat, const uint32_t textureWidth, const uint32_t textureHeight,
        const SHADER_TEXTURE_SLOT textureSlot, const uint32_t numTextures, const bool isCubeTexture, const bool isDepthTexture, const bool genMipmaps) :
        mContext(context), mTexture(nullptr), mShaderResourceView(nullptr), mTextureID(gNextTextureID++), mIsCubeTexture(isCubeTexture), mShaderTextureSlot(textureSlot)
    {
        assert(numTextures >= 1);

        D3D11_TEXTURE2D_DESC textureDesc;
        ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
        textureDesc.Width = textureWidth;
        textureDesc.Height = textureHeight;
        textureDesc.ArraySize = numTextures;
        textureDesc.Format = textureFormat;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        textureDesc.MipLevels = genMipmaps ? 0 : 1;
        if (genMipmaps)
        {
            textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
            textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
        }
        if (isDepthTexture)
            textureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
        if (isCubeTexture)
            textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
        DXCALL(device->CreateTexture2D(&textureDesc, NULL, &mTexture));

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        srvDesc.Format = textureDesc.Format;
        if (isCubeTexture)
        {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            if (genMipmaps)
                srvDesc.TextureCube.MipLevels = -1;
        }
        else if (numTextures > 1)
        {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            srvDesc.Texture2DArray.ArraySize = numTextures;
            if (genMipmaps)
                srvDesc.Texture2DArray.MipLevels = -1;
        }
        else
        {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            if (genMipmaps)
                srvDesc.Texture2D.MipLevels = -1;
        }
        DXCALL(device->CreateShaderResourceView(mTexture, &srvDesc, &mShaderResourceView));

        if (genMipmaps)
        {
            // mip-level 0 data
            uint32_t sizeWidth = textureWidth * sizeof(uint8_t) * 4;
            for (uint32_t index = 0; index < numTextures; ++index)
            {
                const uint32_t subResourceID = D3D11CalcSubresource(0, index, GetNumMipLevels(textureWidth, textureHeight));
                context->UpdateSubresource(mTexture, subResourceID, NULL, &textureData.at(sizeWidth * textureHeight * index), sizeWidth, 0);
            }

            context->GenerateMips(mShaderResourceView);
        }
    }

    DX11Texture::~DX11Texture()
    {
    }


    void DX11Texture::Bind()
    {
        mContext->PSSetShaderResources(mShaderTextureSlot, 1, &mShaderResourceView.p);
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
