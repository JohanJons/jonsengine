#pragma once

#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Types.h"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11AmbientPass
    {
    public:
        DX11AmbientPass(ID3D11Device* device);
        ~DX11AmbientPass();

        void Render(ID3D11DeviceContext* context, const Vec4& ambientLight);


    private:
        struct AmbientCBuffer
        {
            Vec4 mAmbientLight;


            AmbientCBuffer(const Vec4& ambientLight) : mAmbientLight(ambientLight)
            {
            }
        };

        ID3D11VertexShaderPtr mVertexShader;
        ID3D11PixelShaderPtr mPixelShader;
        DX11ConstantBuffer<AmbientCBuffer> mConstantBuffer;
    };
}
