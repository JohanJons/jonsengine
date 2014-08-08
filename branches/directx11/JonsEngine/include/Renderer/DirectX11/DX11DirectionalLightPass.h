#pragma once

#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Types.h"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11DirectionalLightPass
    {
    public:
        DX11DirectionalLightPass(ID3D11Device* device);
        ~DX11DirectionalLightPass();

        void BindForShading(ID3D11DeviceContext* context);
        void Render(ID3D11DeviceContext* context, const Vec4& lightColor, const Vec3& lightDir);


    private:
        struct DirectionalLightCBuffer
        {
            Vec4 mLightColor;
            Vec4 mLightDirection;


            DirectionalLightCBuffer(const Vec4& lightColor, const Vec3& lightDir) : mLightColor(lightColor), mLightDirection(Vec4(-lightDir, 0.0f))
            {
            }
        };

        ID3D11VertexShaderPtr mVertexShader;
        ID3D11PixelShaderPtr mPixelShader;
        DX11ConstantBuffer<DirectionalLightCBuffer> mConstantBuffer;
    };
}
