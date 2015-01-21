#pragma once

#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Shadowmap.h"
#include "include/Renderer/RenderQueue.h"
#include "include/Core/Types.h"

#include <d3d11.h>
#include <vector>
#include <array>

namespace JonsEngine
{
    class DX11VertexTransformPass;

    class DX11PointLightPass
    {
    public:
        const static uint32_t TEXTURE_CUBE_NUM_FACES = 6;

        DX11PointLightPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11VertexTransformPass& vertexTransformPass, const uint32_t shadowmapSize);
        ~DX11PointLightPass();

        void Render(const RenderablePointLight& pointLight, const Mat4& viewMatrix, const Mat4& invProjMatrix, const Vec2& screenSize, const float zFar, const float zNear);

        void BindForShading();


    private:
        struct PointLightCBuffer
        {
            Mat4 mInvProjMatrix;
            Vec4 mLightColor;
            Vec4 mLightPosition;
            Vec2 mScreenSize;
            float mLightIntensity;
            float mMaxDistance;
            float mZFar;
            float mZNear;
            float __padding[2];


            PointLightCBuffer(const Mat4& invProjMatrix, const Vec4& lightColor, const Vec4& lightPosition, const Vec2& screenSize, const float lightIntensity, const float maxDistance, const float zFar, const float zNear) :
                mInvProjMatrix(invProjMatrix), mLightColor(lightColor), mLightPosition(lightPosition), mScreenSize(screenSize), mLightIntensity(lightIntensity), mMaxDistance(maxDistance), mZFar(zFar), mZNear(zNear)
            {
            }
        };

        ID3D11DeviceContextPtr mContext;
        ID3D11PixelShaderPtr mPixelShader;
        ID3D11DepthStencilStatePtr mDSSStencilPass;
        ID3D11DepthStencilStatePtr mDSSShadingPass;
        ID3D11RasterizerStatePtr mRSCullFront;
        ID3D11RasterizerStatePtr mRSNoCulling;

        DX11Mesh mSphereMesh;
        DX11VertexTransformPass& mVertexTransformPass;
        DX11Shadowmap mShadowmap;
        DX11ConstantBuffer<PointLightCBuffer> mPointLightCBuffer;
    };
}
