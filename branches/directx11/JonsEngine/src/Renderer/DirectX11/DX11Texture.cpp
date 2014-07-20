#include "include/Renderer/DirectX11/DX11Texture.h"

#include "include/Renderer/DirectX11/DX11Utils.h"

namespace JonsEngine
{
    static TextureID gNextTextureID = 1;


    DX11Texture::TextureRegister TextureTypeToRegister(const TextureType textureType)
    {
        switch (textureType)
        {
            case TextureType::TEXTURE_TYPE_DIFFUSE: return DX11Texture::TEXTURE_REGISTER_DIFFUSE;
            case TextureType::TEXTURE_TYPE_NORMAL:  return DX11Texture::TEXTURE_REGISTER_NORMAL;
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

        // mip-level 0 data
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

        DXCALL(context->Map(mTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
        std::memcpy(mappedResource.pData, &textureData.at(0), sizeof(uint8_t) * 4 * textureWidth * textureHeight);
        context->Unmap(mTexture, 0);

        DXCALL(device->CreateShaderResourceView(mTexture, NULL, &mShaderResourceView));
        
        context->GenerateMips(mShaderResourceView);
    }

    DX11Texture::~DX11Texture()
    {
        mTexture->Release();
        mShaderResourceView->Release();
    }


    void DX11Texture::Activate(ID3D11DeviceContext* context)
    {
        context->PSSetShaderResources(TextureTypeToRegister(mTextureType), 1, &mShaderResourceView);
    }

    TextureID DX11Texture::GetTextureID() const
    {
        return mTextureID;
    }
}