#pragma once

#include "include/Core/Types.h"

#include <d3d11.h>
#include <vector>
#include <memory>

namespace JonsEngine
{
    class DX11Texture;
    typedef std::shared_ptr<DX11Texture> DX11TexturePtr;

    class DX11Texture
    {
    public:
        enum SHADER_TEXTURE_SLOT
        {
            SHADER_TEXTURE_SLOT_POSITION = 0,
            SHADER_TEXTURE_SLOT_DIFFUSE,
            SHADER_TEXTURE_SLOT_NORMAL,
            SHADER_TEXTURE_SLOT_DEPTH
        };


        DX11Texture(ID3D11Device* device, ID3D11DeviceContext* context, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, TextureType textureType);
        ~DX11Texture();

        void Bind(ID3D11DeviceContext* context, uint32_t textureSlot);
        TextureID GetTextureID() const;


    private:
        ID3D11Texture2D* mTexture;
        ID3D11ShaderResourceView* mShaderResourceView;
        TextureID mTextureID;
        TextureType mTextureType;
    };
}