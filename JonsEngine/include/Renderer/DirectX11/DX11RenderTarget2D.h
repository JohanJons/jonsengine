#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Types.h"
#include "include/Core/Platform/Directx11.h"

#include <array>

namespace JonsEngine
{
    struct DX11RenderTarget2D
    {
    public:
        DX11RenderTarget2D(ID3D11DevicePtr device, const DXGI_FORMAT textureFormat, const uint32_t textureWidth, const uint32_t textureHeight, const bool createUAV);
        ~DX11RenderTarget2D();


        ID3D11Texture2DPtr mTexture;
        ID3D11RenderTargetViewPtr mRTV;
        ID3D11ShaderResourceViewPtr mSRV;
        ID3D11UnorderedAccessViewPtr mUAV;
    };
}
