#pragma once

#include "Renderer/DirectX11/DX11Utils.h"
#include "Core/Types.h"
#include "Core/Platform.h"

namespace JonsEngine
{
    struct DX11DynamicTexture
    {
    public:
        DX11DynamicTexture() = default;
        DX11DynamicTexture(ID3D11DevicePtr device, DXGI_FORMAT textureFormat, uint32_t textureWidth, uint32_t textureHeight, bool createUAV);


        ID3D11Texture2DPtr mTexture = nullptr;
        ID3D11RenderTargetViewPtr mRTV = nullptr;
        ID3D11ShaderResourceViewPtr mSRV = nullptr;
        ID3D11UnorderedAccessViewPtr mUAV = nullptr;
    };
}
