#pragma once

#include "include/Core/Types.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"

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

        ID3D11VertexShader* mVertexShader;
        ID3D11PixelShader* mPixelShader;
        DX11ConstantBuffer<AmbientCBuffer> mConstantBuffer;
    };
}