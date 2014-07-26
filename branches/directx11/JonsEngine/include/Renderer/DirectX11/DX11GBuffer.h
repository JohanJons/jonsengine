#pragma once

#include "include/Core/Types.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11ConstantBufferDefinitions.h"

#include <d3d11.h>

namespace JonsEngine
{
    class GBuffer
    {
    public:
        enum GBUFFER_RENDERTARGET_INDEX
        {
            GBUFFER_RENDERTARGET_INDEX_POSITION = 0,
            GBUFFER_RENDERTARGET_INDEX_DIFFUSE,
            GBUFFER_NUM_RENDERTARGETS
        };


        GBuffer(ID3D11Device* device, IDXGISwapChain* swapChain);
        ~GBuffer();

        void SetConstantData(ID3D11DeviceContext* context, const Mat4& wvpMatrix, const Mat4& worldMatrix, const float textureTilingFactor, const bool hasDiffuseTexture);
        void BindForDrawing(ID3D11DeviceContext* context);
        void BindForReading(ID3D11DeviceContext* context);


    private:
        ID3D11Texture2D* mGeometryTextures[GBuffer::GBUFFER_NUM_RENDERTARGETS];
        ID3D11RenderTargetView* mRenderTargets[GBuffer::GBUFFER_NUM_RENDERTARGETS];
        ID3D11Texture2D* mDepthStencilBuffer;
        ID3D11DepthStencilView* mDepthStencilView;
        ID3D11DepthStencilState* mDepthStencilState;
        ID3D11InputLayout* mInputLayout;
        ID3D11VertexShader* mVertexShader;
        ID3D11PixelShader* mPixelShader;
        DX11ConstantBuffer<ConstantBufferGBuffer> mConstantBuffer;
    };
}