#pragma once

#include "include/Renderer/RenderCommands.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
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

        DX11PointLightPass(ID3D11Device* device, DX11Backbuffer& backbuffer, uint32_t shadowmapSize);
        ~DX11PointLightPass();

        void BindForShading(ID3D11DeviceContext* context);
        void Render(ID3D11DeviceContext* context, const RenderQueue& renderQueue, std::vector<DX11MeshPtr>& meshes, ID3D11DepthStencilView* gbufferDSV, const RenderableLighting::PointLight& pointLight);


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
            float mLightIntensity;
            float mMaxDistance;
            float __padding[2];


            PointLightCBuffer(const Mat4 lightWVPMatrix, const Vec4& lightColor, const Vec4& lightPosition, const float lightIntensity, const float maxDistance) :
                mLightWVPMatrix(lightWVPMatrix), mLightColor(lightColor), mLightPosition(lightPosition), mLightIntensity(lightIntensity), mMaxDistance(maxDistance)
            {
            }
        };

        void DepthPass(ID3D11DeviceContext* context, const RenderQueue& renderQueue, std::vector<DX11MeshPtr>& meshes, const Mat4& lightVPMatrix);

        ID3D11VertexShaderPtr mShadingVertexShader;
        ID3D11VertexShaderPtr mNullVertexShader;
        ID3D11PixelShaderPtr mPixelShader;
        ID3D11InputLayoutPtr mInputLayout;
        ID3D11DepthStencilStatePtr mDSSStencilPass;
        ID3D11DepthStencilStatePtr mDSSShadingPass;
        ID3D11RasterizerStatePtr mRSCullFront;
        ID3D11RasterizerStatePtr mRSNoCulling;
        ID3D11Texture2DPtr mShadowmapTexture;
        std::array<ID3D11DepthStencilViewPtr, TEXTURE_CUBE_NUM_FACES> mShadowmapView;
        ID3D11ShaderResourceViewPtr mShadowmapSRV;
        DX11Mesh mSphereMesh;
        DX11Backbuffer& mBackbuffer;
        D3D11_VIEWPORT mShadowPassViewport;
        DX11ConstantBuffer<NullCBuffer> mNullCBuffer;
        DX11ConstantBuffer<PointLightCBuffer> mPointLightCBuffer;
    };
}
