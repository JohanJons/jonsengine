#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/Shaders/Constants.h"
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
            SHADER_TEXTURE_SLOT_DIFFUSE = TEXTURE_SLOT_DIFFUSE,
            SHADER_TEXTURE_SLOT_NORMAL = TEXTURE_SLOT_NORMAL,
            SHADER_TEXTURE_SLOT_DEPTH = TEXTURE_SLOT_DEPTH,
            SHADER_TEXTURE_SLOT_EXTRA = TEXTURE_SLOT_EXTRA,
            NUM_SHADER_TEXTURE_SLOTS
        };

        DX11Texture(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::vector<uint8_t>& textureData, const uint32_t textureWidth, const uint32_t textureHeight,
            const SHADER_TEXTURE_SLOT textureSlot, const bool isSRGB);
        ~DX11Texture();

        void Bind();

        TextureID GetTextureID() const;


    private:
        ID3D11DeviceContextPtr mContext;
        ID3D11Texture2DPtr mTexture;
        ID3D11ShaderResourceViewPtr mShaderResourceView;
        TextureID mTextureID;
        SHADER_TEXTURE_SLOT mShaderTextureSlot;
    };
}
