#pragma once

#include "include/Core/Types.h"
#include "include/Renderer/RenderCommands.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"

#include <d3d11.h>
#include <vector>

namespace JonsEngine
{
    class DX11BackBuffer;

    class DX11PointLightPass
    {
    public:
        DX11PointLightPass(ID3D11Device* device, DX11BackBuffer& backbuffer, uint32_t shadowmapSize);
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

        ID3D11VertexShader* mShadingVertexShader;
        ID3D11VertexShader* mNullVertexShader;
        ID3D11PixelShader* mPixelShader;
        ID3D11InputLayout* mInputLayout;
        ID3D11DepthStencilState* mDSSStencilPass;
        ID3D11DepthStencilState* mDSSShadingPass;
        ID3D11RasterizerState* mRSCullFront;
        ID3D11RasterizerState* mRSNoCulling;
        ID3D11Texture2D* mShadowmapTexture;
        ID3D11DepthStencilView* mShadowmapView[6];
        ID3D11ShaderResourceView* mShadowmapSRV;
        DX11Mesh mSphereMesh;
        DX11BackBuffer& mBackBuffer;
        D3D11_VIEWPORT mShadowPassViewport;
        DX11ConstantBuffer<NullCBuffer> mNullCBuffer;
        DX11ConstantBuffer<PointLightCBuffer> mPointLightCBuffer;
    };
}