#pragma once

#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
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

        void Render(ID3D11DeviceContextPtr context, const Vec4& ambientLight);


    private:
        struct AmbientCBuffer
        {
            Vec4 mAmbientLight;


            AmbientCBuffer(const Vec4& ambientLight) : mAmbientLight(ambientLight)
            {
            }
        };

        DX11FullscreenTrianglePass& mFullscreenPass;
        ID3D11PixelShaderPtr mPixelShader;
        DX11ConstantBuffer<AmbientCBuffer> mConstantBuffer;
    };
}
