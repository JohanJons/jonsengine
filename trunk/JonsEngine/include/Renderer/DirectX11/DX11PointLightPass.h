#pragma once

#include "include/Renderer/RenderCommands.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11VertexTransformPass.h"
#include "include/Renderer/DirectX11/DX11Shadowmap.h"
#include "include/Core/Types.h"

#include <d3d11.h>
#include <vector>
#include <array>

namespace JonsEngine
{
    class DX11Backbuffer;

    class DX11PointLightPass
    {
    public:
        const static uint32_t TEXTURE_CUBE_NUM_FACES = 6;

        DX11PointLightPass(ID3D11DevicePtr device, DX11Backbuffer& backbuffer, DX11VertexTransformPass& vertexTransformPass, const uint32_t shadowmapSize);
        ~DX11PointLightPass();

        void Render(ID3D11DeviceContextPtr context, const RenderQueue& renderQueue, std::vector<DX11MeshPtr>& meshes, const RenderableLighting::PointLight& pointLight, const float zFar, const float zNear);

        void BindForShading(ID3D11DeviceContextPtr context);


    private:
        struct PointLightCBuffer
        {
            Vec4 mLightColor;
            Vec4 mLightPosition;
            float mLightIntensity;
            float mMaxDistance;
            float mZFar;
            float mZNear;


            PointLightCBuffer(const Vec4& lightColor, const Vec4& lightPosition, const float lightIntensity, const float maxDistance, const float zFar, const float zNear) :
                mLightColor(lightColor), mLightPosition(lightPosition), mLightIntensity(lightIntensity), mMaxDistance(maxDistance), mZFar(zFar), mZNear(zNear)
            {
            }
        };

        ID3D11PixelShaderPtr mPixelShader;
        ID3D11DepthStencilStatePtr mDSSStencilPass;
        ID3D11DepthStencilStatePtr mDSSShadingPass;
        ID3D11RasterizerStatePtr mRSCullFront;
        ID3D11RasterizerStatePtr mRSNoCulling;
        DX11Mesh mSphereMesh;
        DX11Backbuffer& mBackbuffer;
        DX11VertexTransformPass& mVertexTransformPass;
        DX11Shadowmap mShadowmap;
        DX11ConstantBuffer<PointLightCBuffer> mPointLightCBuffer;
    };
}
