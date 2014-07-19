#include "include/Renderer/DirectX11/DX11Texture.h"

#include "include/Renderer/DirectX11/DX11Utils.h"

namespace JonsEngine
{
    static TextureID gNextTextureID = 1;


    DX11Texture::DX11Texture(ID3D11Device* device, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ColorFormat colorFormat, TextureType textureType, Logger& logger) :
        mTexture(nullptr), mTextureID(gNextTextureID++)
    {
        D3D11_TEXTURE2D_DESC textureDesc;
        ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
        textureDesc.Width = textureWidth;
        textureDesc.Height = textureHeight;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Usage = D3D11_USAGE_DYNAMIC;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        D3D11_SUBRESOURCE_DATA initData;
        ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
        initData.pSysMem = &textureData.at(0);

        DXCALL(device->CreateTexture2D(&textureDesc, &initData, &mTexture));
    }

    DX11Texture::~DX11Texture()
    {
        mTexture->Release();
    }


    TextureID DX11Texture::GetTextureID() const
    {
        return mTextureID;
    }
}