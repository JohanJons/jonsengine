#pragma once

#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11BoxBlurPass.h"
#include "include/Core/Types.h"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11FullscreenTrianglePass;

    class DX11AmbientPass
    {
    public:
        DX11AmbientPass(ID3D11DevicePtr device, DX11FullscreenTrianglePass& fullscreenPass);
        ~DX11AmbientPass();

        void Render(ID3D11DeviceContextPtr context, const Mat4& viewMatrix, const Vec4& ambientLight, const Vec2& screenSize, const bool useSSAO);


    private:
        struct AmbientCBuffer
        {
            Vec4 mAmbientLight;


            AmbientCBuffer(const Vec4& ambientLight) : mAmbientLight(ambientLight)
            {
            }
        };

        struct SSAOCBuffer
        {
            Mat4 mViewMatrix;


            SSAOCBuffer(const Mat4& viewMatrix) : mViewMatrix(viewMatrix)
            {
            }
        };

        DX11FullscreenTrianglePass& mFullscreenPass;
        DX11BoxBlurPass mBoxBlurPass;
        ID3D11PixelShaderPtr mAmbientPixelShader;
        DX11ConstantBuffer<AmbientCBuffer> mAmbientCBuffer;

        ID3D11PixelShaderPtr mSSAOPixelShader;
        DX11ConstantBuffer<SSAOCBuffer> mSSAOCBuffer;
        ID3D11Texture2DPtr mSSAOTexture;
        ID3D11ShaderResourceViewPtr mSSAOSRV;
        ID3D11RenderTargetViewPtr mSSAORTV;
    };
}
