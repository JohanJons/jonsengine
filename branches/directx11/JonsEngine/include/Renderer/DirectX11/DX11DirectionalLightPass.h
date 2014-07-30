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
        void Render(ID3D11DeviceContext* context, const Mat4& lightVPMatrix, const Vec4& lightColor, const Vec3& lightDir, uint32_t screenWidth, uint32_t screenHeight);


    private:
        struct DirectionalLightCBuffer
        {
            Mat4 mLightViewProjectionMatrix;
            Vec4 mLightColor;
            Vec3 mLightDirection;
            Vec2 mScreenSize;
            float _padding[3];


            DirectionalLightCBuffer(const Mat4& lightVPMatrix, const Vec4& lightColor, const Vec3& lightDir, const Vec2& screenSize) :
                mLightViewProjectionMatrix(lightVPMatrix), mLightColor(lightColor), mLightDirection(lightDir), mScreenSize(screenSize)
            {
            }
        };

        ID3D11VertexShader* mVertexShader;
        ID3D11PixelShader* mPixelShader;
        DX11ConstantBuffer<DirectionalLightCBuffer> mConstantBuffer;
    };
}