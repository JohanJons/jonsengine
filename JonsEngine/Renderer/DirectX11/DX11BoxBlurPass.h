#pragma once

#include "DX11ConstantBuffer.hpp"
#include "DX11Utils.h"
#include "Types.h"
#include "Platform.h"

namespace JonsEngine
{
    class DX11FullscreenTrianglePass;

    class DX11BoxBlurPass
    {
    public:
        DX11BoxBlurPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11FullscreenTrianglePass& fullscreenPass, const DXGI_FORMAT textureFormat, const uint16_t screenWidth, const uint16_t screenHeight);
        ~DX11BoxBlurPass();

        void Render(ID3D11ShaderResourceViewPtr textureToBlur, const Vec2& windowSize);


    private:
        struct BoxBlurCBuffer
        {
            Vec2 mTexelScale;
            float __padding[2];


            BoxBlurCBuffer(const Vec2& texelScale) : mTexelScale(texelScale)
            {
            }
        };

        ID3D11DeviceContextPtr mContext;
        ID3D11PixelShaderPtr mPixelShader;
        ID3D11Texture2DPtr mBoxBlurTexture;
        ID3D11ShaderResourceViewPtr mBoxBlurSRV;
        ID3D11RenderTargetViewPtr mBoxBlurRTV;

        DX11FullscreenTrianglePass& mFullscreenPass;
        DX11ConstantBuffer<BoxBlurCBuffer> mBoxBlurCBuffer;
    };
}
