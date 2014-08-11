#pragma once

#include "include/Renderer/RenderCommands.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11NullPass.h"
#include "include/Core/Types.h"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11Backbuffer;

    class DX11DirectionalLightPass
    {
    public:
        static const uint32_t NUM_SHADOWMAP_CASCADES = 4;

        DX11DirectionalLightPass(ID3D11DevicePtr device, DX11Backbuffer& backbuffer, uint32_t shadowmapSize);
        ~DX11DirectionalLightPass();

        void BindForShading(ID3D11DeviceContextPtr context);
        void Render(const RenderQueue& renderQueue, std::vector<DX11MeshPtr>& meshes, const float degreesFOV, const float aspectRatio, ID3D11DeviceContextPtr context, const Vec4& lightColor, const Vec3& lightDir);


    private:
        struct DirectionalLightCBuffer
        {
            Vec4 mLightColor;
            Vec4 mLightDirection;


            DirectionalLightCBuffer(const Vec4& lightColor, const Vec3& lightDir) : mLightColor(lightColor), mLightDirection(Vec4(-lightDir, 0.0f))
            {
            }
        };

        ID3D11Texture2DPtr mShadowmapTexture;
        std::array<ID3D11DepthStencilViewPtr, NUM_SHADOWMAP_CASCADES> mShadowmapView;
        ID3D11ShaderResourceViewPtr mShadowmapSRV;
        D3D11_VIEWPORT mShadowPassViewport;
        ID3D11VertexShaderPtr mVertexShader;
        ID3D11PixelShaderPtr mPixelShader;
        DX11NullPass mNullPass;
        DX11Backbuffer& mBackbuffer;
        DX11ConstantBuffer<DirectionalLightCBuffer> mConstantBuffer;
    };
}
