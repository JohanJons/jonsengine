#pragma once

#include "include/Core/Types.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11DirectionalLightPass
    {
    public:
        DX11DirectionalLightPass(ID3D11Device* device);
        ~DX11DirectionalLightPass();


    private:
        struct DirectionalLightCBuffer
        {
            Mat4 mLightViewProjectionMatrix;
            Vec4 mLightColor;
            Vec4 mLightDirection;
            Vec2 mScreenSize;
            float _padding[2];
        };

        ID3D11VertexShader* mVertexShader;
        ID3D11PixelShader* mPixelShader;
        DX11ConstantBuffer<DirectionalLightCBuffer> mConstantBuffer;
    };
}