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
        DX11Texture(ID3D11Device* device, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ColorFormat colorFormat, TextureType textureType, Logger& logger);
        ~DX11Texture();

        TextureID GetTextureID() const;


    private:
        ID3D11Texture2D* mTexture;
        TextureID mTextureID;
    };
}