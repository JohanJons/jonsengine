#include "include/Renderer/DirectX11/DX11Texture.h"

#include "include/Renderer/DirectX11/DX11Utils.h"

namespace JonsEngine
{
    static TextureID gNextTextureID = 1;


    uint32_t TextureTypeToSlot(const TextureType textureType)
    {
        switch (textureType)
        {
            case TextureType::TEXTURE_TYPE_DIFFUSE: return 0;
            case TextureType::TEXTURE_TYPE_NORMAL:  return 1;
            default:                                return 0;
        }
    }


    DX11Texture::DX11Texture(ID3D11Device* device, ID3D11DeviceContext* context, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, TextureType textureType, Logger& logger) :
        mTexture(nullptr), mShaderResourceView(nullptr), mTextureID(gNextTextureID++), mTextureType(textureType)
    {
        // create texture
        D3D11_TEXTURE2D_DESC textureDesc;
        ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
        textureDesc.Width = textureWidth;
        textureDesc.Height = textureHeight;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
        DXCALL(device->CreateTexture2D(&textureDesc, NULL, &mTexture));

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        srvDesc.Format = textureDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = -1;
        DXCALL(device->CreateShaderResourceView(mTexture, &srvDesc, &mShaderResourceView));

        // mip-level 0 data
        uint32_t sizeWidth = textureWidth * sizeof(uint8_t) * 4;
        context->UpdateSubresource(mTexture, 0, NULL, &textureData.at(0), textureData.size() /*textureHeight * sizeWidth*/, 0);

        context->GenerateMips(mShaderResourceView);
    }

    DX11Texture::~DX11Texture()
    {
        mShaderResourceView->Release();
        mTexture->Release();
    }


    void DX11Texture::Activate(ID3D11DeviceContext* context)
    {
        context->PSSetShaderResources(TextureTypeToSlot(mTextureType), 1, &mShaderResourceView);
    }

    TextureID DX11Texture::GetTextureID() const
    {
        return mTextureID;
    }
}