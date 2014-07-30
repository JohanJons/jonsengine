#pragma once

#include "include/Core/Types.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11PointLightPass
    {
    public:
        DX11PointLightPass(ID3D11Device* device);
        ~DX11PointLightPass();


    private:
        struct PointLightCBuffer
        {
            Mat4 mLightViewProjectionMatrix;
        };

        ID3D11VertexShader* mVertexShader;
        ID3D11PixelShader* mPixelShader;
        DX11ConstantBuffer<PointLightCBuffer> mConstantBuffer;
    };
}