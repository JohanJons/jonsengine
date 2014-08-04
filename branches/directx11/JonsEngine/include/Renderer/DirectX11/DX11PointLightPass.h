#pragma once

#include "include/Core/Types.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11PointLightPass
    {
    public:
        DX11PointLightPass(ID3D11Device* device);
        ~DX11PointLightPass();

        void BindForShading(ID3D11DeviceContext* context);
        void Render(ID3D11DeviceContext* context, const Mat4& lightWVP, const Vec4& lightColor, const Vec3& lightPosition, const float lightIntensity, const float maxDistance, uint32_t screenWidth, uint32_t screenHeight);


    private:
        struct NullCBuffer
        {
            Mat4 mWVPMatrix;


            NullCBuffer(const Mat4& wvpMatrix) : mWVPMatrix(wvpMatrix)
            {
            }
        };

        struct PointLightCBuffer
        {
            Mat4 mLightWVPMatrix;
            Vec4 mLightColor;
            Vec4 mLightPosition;
            Vec2 mScreenSize;
            float mLightIntensity;
            float mMaxDistance;


            PointLightCBuffer(const Mat4 lightWVPMatrix, const Vec4& lightColor, const Vec4& lightPosition, const Vec2& screenSize, const float lightIntensity, const float maxDistance) :
                mLightWVPMatrix(lightWVPMatrix), mLightColor(lightColor), mLightPosition(lightPosition), mScreenSize(screenSize), mLightIntensity(lightIntensity), mMaxDistance(maxDistance)
            {
            }
        };

        ID3D11VertexShader* mShadingVertexShader;
        ID3D11VertexShader* mStencilVertexShader;
        ID3D11PixelShader* mPixelShader;
        ID3D11InputLayout* mInputLayout;
        ID3D11DepthStencilState* mDSSStencilPass;
        ID3D11DepthStencilState* mDSSShadingPass;
        ID3D11RasterizerState* mRSCullFront;
        ID3D11RasterizerState* mRSNoCulling;
        DX11Mesh mSphereMesh;
        DX11ConstantBuffer<NullCBuffer> mNullCBuffer;
        DX11ConstantBuffer<PointLightCBuffer> mPointLightCBuffer;
    };
}