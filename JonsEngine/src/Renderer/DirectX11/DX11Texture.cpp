#include "include/Renderer/DirectX11/DX11Texture.h"

#include "include/Renderer/DirectX11/DX11Utils.h"

namespace JonsEngine
{
    static MaterialID gNextTextureID = 1;

    uint32_t GetNumMipLevels(uint32_t width, uint32_t height);
    D3D11_TEXTURE2D_DESC GetTextureDesc(ID3D11Texture2DPtr texture);


    DX11Texture::DX11Texture(IDXGISwapChainPtr swapchain, ID3D11DeviceContextPtr context) :
        mTextureDimension(TextureDimension::Texture2D),
        mContext(context),
        mTexture(nullptr)
    {
        DXCALL(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&mTexture));
    }

    DX11Texture::DX11Texture(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const DXGI_FORMAT textureFormat, const uint32_t textureWidth, const uint32_t textureHeight,
        const uint32_t numTextures, const TextureDimension dimension, const bool isRenderTarget, const bool isDepthTexture) :
        DX11Texture(device, context, textureFormat, textureWidth, textureHeight, numTextures, dimension, isRenderTarget, isDepthTexture, std::vector<uint8_t>(), false)
    {
    }

    DX11Texture::DX11Texture(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const DXGI_FORMAT textureFormat, const uint32_t textureWidth, const uint32_t textureHeight, const uint32_t numTextures,
        const TextureDimension dimension, const bool isRenderTarget, const bool isDepthTexture, const std::vector<uint8_t>& textureData, const bool genMipmaps) :
        mContext(context), mTexture(nullptr), mTextureDimension(dimension)
    {
        assert(numTextures >= 1);
        assert(mTextureDimension == TextureDimension::Texture2D || mTextureDimension == TextureDimension::Texture2DArray || mTextureDimension == TextureDimension::TextureCube);

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
        if (isRenderTarget || genMipmaps)
            textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
        if (genMipmaps)
            textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
        if (isDepthTexture)
            textureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
        if (dimension == TextureDimension::TextureCube)
            textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
        DXCALL(device->CreateTexture2D(&textureDesc, NULL, &mTexture));

        // fill mip-level 0 data
        if (!textureData.empty())
        {
            uint32_t sizeWidth = textureWidth * sizeof(uint8_t) * 4;
            for (uint32_t index = 0; index < numTextures; ++index)
            {
                const uint32_t subResourceID = D3D11CalcSubresource(0, index, GetNumMipLevels(textureWidth, textureHeight));
                context->UpdateSubresource(mTexture, subResourceID, NULL, &textureData.at(sizeWidth * textureHeight * index), sizeWidth, 0);
            }
        }

        // generate mipmaps
        if (genMipmaps)
        {
            assert(!textureData.empty());

            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
            ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
            srvDesc.Format = textureDesc.Format;
            if (dimension == TextureDimension::TextureCube)
            {
                srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
                srvDesc.TextureCube.MipLevels = -1;
            }
            else if (dimension == TextureDimension::Texture2DArray)
            {
                srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                srvDesc.Texture2DArray.ArraySize = numTextures;
                srvDesc.Texture2DArray.MipLevels = -1;
            }
            else
            {
                srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                srvDesc.Texture2D.MipLevels = -1;
            }

            // create temporary SRV for creating mipmaps
            ID3D11ShaderResourceViewPtr tempSRV(nullptr);
            DXCALL(device->CreateShaderResourceView(mTexture, &srvDesc, &tempSRV));
            context->GenerateMips(tempSRV);
        }
    }

    DX11Texture::~DX11Texture()
    {
    }
    

    ID3D11ShaderResourceViewPtr DX11Texture::CreateSRV(ID3D11DevicePtr device)
    {
        ID3D11ShaderResourceViewPtr retSRV(nullptr);
        DXCALL(device->CreateShaderResourceView(mTexture, nullptr, &retSRV));

        return retSRV;
    }

    ID3D11ShaderResourceViewPtr DX11Texture::CreateSRV(ID3D11DevicePtr device, const DXGI_FORMAT srvFormat)
    {
        assert(mTextureDimension == TextureDimension::Texture2D || mTextureDimension == TextureDimension::Texture2DArray || mTextureDimension == TextureDimension::TextureCube);

        D3D11_TEXTURE2D_DESC textureDesc(GetTextureDesc(mTexture));

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        srvDesc.Format = srvFormat;
        if (mTextureDimension == TextureDimension::TextureCube)
        {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            srvDesc.TextureCube.MipLevels = -1;
        }
        else if (mTextureDimension == TextureDimension::Texture2DArray)
        {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            srvDesc.Texture2DArray.ArraySize = textureDesc.ArraySize;
            srvDesc.Texture2DArray.MipLevels = -1;
        }
        else
        {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = -1;
        }

        ID3D11ShaderResourceViewPtr retSRV(nullptr);
        DXCALL(device->CreateShaderResourceView(mTexture, &srvDesc, &retSRV));

        return retSRV;
    }

    ID3D11RenderTargetViewPtr DX11Texture::CreateRTV(ID3D11DevicePtr device)
    {
        ID3D11RenderTargetViewPtr retRTV(nullptr);
        DXCALL(device->CreateRenderTargetView(mTexture, nullptr, &retRTV));

        return retRTV;
    }

    ID3D11RenderTargetViewPtr DX11Texture::CreateRTV(ID3D11DevicePtr device, const DXGI_FORMAT srvFormat)
    {
        // TODO: support other dimensions?
        assert(mTextureDimension == TextureDimension::Texture2D);

        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
        ZeroMemory(&rtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
        rtvDesc.Format = srvFormat;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        
        ID3D11RenderTargetViewPtr retRTV(nullptr);
        DXCALL(device->CreateRenderTargetView(mTexture, &rtvDesc, &retRTV));

        return retRTV;
    }

    ID3D11UnorderedAccessViewPtr DX11Texture::CreateUAV(ID3D11DevicePtr device)
    {
        ID3D11UnorderedAccessViewPtr retUAV(nullptr);
        DXCALL(device->CreateUnorderedAccessView(mTexture, nullptr, &retUAV));

        return retUAV;
    }
    
    ID3D11DepthStencilViewPtr DX11Texture::CreateDSV(ID3D11DevicePtr device)
    {
        ID3D11DepthStencilViewPtr retDSV(nullptr);
        DXCALL(device->CreateDepthStencilView(mTexture, nullptr, &retDSV));

        return retDSV;
    }


    void DX11Texture::CopyTexture(ID3D11Texture2DPtr dest)
    {
        mContext->CopyResource(dest, mTexture);
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

    D3D11_TEXTURE2D_DESC GetTextureDesc(ID3D11Texture2DPtr texture)
    {
        D3D11_TEXTURE2D_DESC textureDesc;
        ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
        texture->GetDesc(&textureDesc);

        return textureDesc;
    }
}
