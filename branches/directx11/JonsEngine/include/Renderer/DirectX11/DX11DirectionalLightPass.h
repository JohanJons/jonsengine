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

        void BindForDrawing(ID3D11DeviceContext* context);
        void Render(ID3D11DeviceContext* context, const Vec4& lightColor, const Vec3& lightDir, uint32_t screenWidth, uint32_t screenHeight);


    private:
        struct DirectionalLightCBuffer
        {
            Vec4 mLightColor;
            Vec4 mLightDirection;
            Vec2 mScreenSize;
            float __padding[2];


            DirectionalLightCBuffer(const Vec4& lightColor, const Vec3& lightDir, const Vec2& screenSize) :
                mLightColor(lightColor), mLightDirection(Vec4(-lightDir, 0.0f)), mScreenSize(screenSize)
            {
            }
        };

        ID3D11VertexShader* mVertexShader;
        ID3D11PixelShader* mPixelShader;
        DX11ConstantBuffer<DirectionalLightCBuffer> mConstantBuffer;
    };
}