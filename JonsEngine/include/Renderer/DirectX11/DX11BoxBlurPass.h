#pragma once

#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Types.h"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11FullscreenTrianglePass;

    class DX11BoxBlurPass
    {
    public:
        DX11BoxBlurPass(ID3D11DevicePtr device, DX11FullscreenTrianglePass& fullscreenPass, const DXGI_FORMAT textureFormat);
        ~DX11BoxBlurPass();

        void Render(ID3D11DeviceContextPtr context, ID3D11ShaderResourceViewPtr textureToBlur, const Vec2& screenSize);


    private:
        struct BoxBlurCBuffer
        {
            Vec2 mTexelScale;
            float __padding[2];


            BoxBlurCBuffer(const Vec2& texelScale) : mTexelScale(texelScale)
            {
            }
        };

        DX11FullscreenTrianglePass& mFullscreenPass;
        DX11ConstantBuffer<BoxBlurCBuffer> mBoxBlurCBuffer;
        ID3D11PixelShaderPtr mPixelShader;
        ID3D11Texture2DPtr mBoxBlurTexture;
        ID3D11ShaderResourceViewPtr mBoxBlurSRV;
        ID3D11RenderTargetViewPtr mBoxBlurRTV;
    };
}
