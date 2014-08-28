#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/Shaders/Constants.hlsl"
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
            SHADER_TEXTURE_SLOT_POSITION = TEXTURE_SLOT_POSITION,
            SHADER_TEXTURE_SLOT_DIFFUSE = TEXTURE_SLOT_DIFFUSE,
            SHADER_TEXTURE_SLOT_NORMAL = TEXTURE_SLOT_NORMAL,
            SHADER_TEXTURE_SLOT_DEPTH = TEXTURE_SLOT_DEPTH
        };

        DX11Texture(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::vector<uint8_t>& textureData, const uint32_t textureWidth, const uint32_t textureHeight, const SHADER_TEXTURE_SLOT textureSlot);
        ~DX11Texture();

        void Bind(ID3D11DeviceContextPtr context);
        TextureID GetTextureID() const;


    private:
        ID3D11Texture2DPtr mTexture;
        ID3D11ShaderResourceViewPtr mShaderResourceView;
        TextureID mTextureID;
        SHADER_TEXTURE_SLOT mShaderTextureSlot;
    };
}