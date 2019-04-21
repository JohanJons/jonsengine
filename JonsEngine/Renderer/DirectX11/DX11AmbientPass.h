#pragma once

#include "Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "Renderer/DirectX11/DX11Utils.h"
#include "Renderer/DirectX11/DX11BoxBlurPass.h"
#include "Core/Platform.h"
#include "Core/Types.h"

namespace JonsEngine
{
    class DX11FullscreenTrianglePass;

    class DX11AmbientPass
    {
    public:
        DX11AmbientPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11FullscreenTrianglePass& fullscreenPass, const uint32_t windowWidth, const uint32_t windowHeight);
        ~DX11AmbientPass();

        void Render(const Vec4& ambientLight, const Vec2& windowSize, const bool useSSAO);


    private:
        struct AmbientCBuffer
        {
            Vec4 mAmbientLight;
            int mUseSSAO;
            float __padding[3];


            AmbientCBuffer(const Vec4& ambientLight, const bool useSSAO) : mAmbientLight(ambientLight), mUseSSAO(useSSAO)
            {
            }
        };

        struct SSAOCBuffer
        {
            Vec2 mWindowSize;
            float __padding[2];


            SSAOCBuffer(const Vec2& windowSize) : mWindowSize(windowSize)
            {
            }
        };

        ID3D11DeviceContextPtr mContext;
        ID3D11PixelShaderPtr mAmbientPixelShader;
        ID3D11PixelShaderPtr mSSAOPixelShader;
        ID3D11Texture2DPtr mSSAOTexture;
        ID3D11ShaderResourceViewPtr mSSAOSRV;
        ID3D11RenderTargetViewPtr mSSAORTV;

        DX11FullscreenTrianglePass& mFullscreenPass;
        DX11BoxBlurPass mBoxBlurPass;
        DX11ConstantBuffer<AmbientCBuffer> mAmbientCBuffer;
        DX11ConstantBuffer<SSAOCBuffer> mSSAOCBuffer;
    };
}
