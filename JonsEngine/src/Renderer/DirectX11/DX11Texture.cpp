#include "include/Renderer/DirectX11/DX11Texture.h"

#include "include/Renderer/DirectX11/DX11Utils.h"

namespace JonsEngine
{
    static TextureID gNextTextureID = 1;


    DX11Texture::DX11Texture(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::vector<uint8_t>& textureData, const uint32_t textureWidth, const uint32_t textureHeight,
        const SHADER_TEXTURE_SLOT textureSlot, const bool isCubeTexture, const bool isSRGB) :
        mContext(context), mTexture(nullptr), mShaderResourceView(nullptr), mTextureID(gNextTextureID++), mIsCubeTexture(isCubeTexture), mShaderTextureSlot(textureSlot)
    {
        const uint32_t cubemapNumTextures = 6;

        // create texture
        D3D11_TEXTURE2D_DESC textureDesc;
        ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
        textureDesc.Width = textureWidth;
        textureDesc.Height = textureHeight;
        textureDesc.ArraySize = isCubeTexture ? cubemapNumTextures : 1;
        if (isSRGB)
            textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        else
            textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
        if (isCubeTexture)
            textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;

        if (isCubeTexture)
        {
            D3D11_SUBRESOURCE_DATA initialData[cubemapNumTextures];
            for (uint32_t face = 0; face < cubemapNumTextures; ++face)
            {
                ZeroMemory(&initialData[face], sizeof(D3D11_SUBRESOURCE_DATA));
                //initialData[face].pSysMem = &textureData.at(0);
                initialData[face].SysMemPitch = textureWidth * sizeof(uint8_t) * 4;
            }
           
            DXCALL(device->CreateTexture2D(&textureDesc, &initialData, &mTexture));
        }
        else
        {
            D3D11_SUBRESOURCE_DATA initialData;
            ZeroMemory(&initialData, sizeof(D3D11_SUBRESOURCE_DATA));
            initialData.pSysMem = &textureData.at(0);
            initialData.SysMemPitch = textureWidth * sizeof(uint8_t) * 4;

            DXCALL(device->CreateTexture2D(&textureDesc, &initialData, &mTexture));
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        srvDesc.Format = textureDesc.Format;
        srvDesc.ViewDimension = isCubeTexture ? D3D11_SRV_DIMENSION_TEXTURECUBE : D3D11_SRV_DIMENSION_TEXTURE2D;
        if (isCubeTexture)
            srvDesc.Texture2D.MipLevels = -1;
        else
            srvDesc.TextureCube.MipLevels = -1;
        DXCALL(device->CreateShaderResourceView(mTexture, &srvDesc, &mShaderResourceView));

        context->GenerateMips(mShaderResourceView);
    }

    DX11Texture::~DX11Texture()
    {
    }


    void DX11Texture::Bind()
    {
        mContext->PSSetShaderResources(mShaderTextureSlot, 1, &mShaderResourceView.p);
    }
}
