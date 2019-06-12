#pragma once

#include "Renderer/DirectX11/DX11Utils.h"
#include "Core/Types.h"
#include "Core/Platform.h"

namespace JonsEngine
{
    // TODO: Merge wutg dx11texture
    struct DX11DynamicTexture
    {
    public:
        DX11DynamicTexture() = default;
        DX11DynamicTexture(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DXGI_FORMAT textureFormat, uint32_t textureWidth, uint32_t textureHeight, bool createUAV);

        void BindAsShaderResource(const SHADER_TEXTURE_SLOT shaderTextureSlot) const;
        void Unbind(const SHADER_TEXTURE_SLOT shaderTextureSlot) const;

        ID3D11DeviceContextPtr mContext;
        ID3D11Texture2DPtr mTexture = nullptr;
        ID3D11RenderTargetViewPtr mRTV = nullptr;
        ID3D11ShaderResourceViewPtr mSRV = nullptr;
        ID3D11UnorderedAccessViewPtr mUAV = nullptr;
    };
}
