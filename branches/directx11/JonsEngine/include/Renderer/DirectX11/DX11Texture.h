#pragma once

#include "include/Core/Types.h"
#include "include/Core/Logging/Logger.h"

#include <d3d11.h>
#include <vector>

namespace JonsEngine
{
    class DX11Texture
    {
    public:
        DX11Texture(ID3D11Device* device, ID3D11DeviceContext* context, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, TextureType textureType, Logger& logger);
        ~DX11Texture();

        void Activate(ID3D11DeviceContext* context, uint32_t textureSlot);
        TextureID GetTextureID() const;


    private:
        ID3D11Texture2D* mTexture;
        ID3D11ShaderResourceView* mShaderResourceView;
        TextureID mTextureID;
        TextureType mTextureType;
    };
}