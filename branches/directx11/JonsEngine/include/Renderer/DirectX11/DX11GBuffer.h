#pragma once

#include "include/Core/Types.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11GBuffer
    {
    public:
        enum GBUFFER_RENDERTARGET_INDEX
        {
            GBUFFER_RENDERTARGET_INDEX_DIFFUSE = 0,
            GBUFFER_RENDERTARGET_INDEX_POSITION,
            GBUFFER_NUM_RENDERTARGETS,
        };


        DX11GBuffer(ID3D11Device* device, uint32_t textureWidth, uint32_t textureHeight);
        ~DX11GBuffer();

        void SetConstantData(ID3D11DeviceContext* context, const Mat4& wvpMatrix, const Mat4& worldMatrix, const float textureTilingFactor, const bool hasDiffuseTexture);
        void BindForDrawing(ID3D11DeviceContext* context);
        void BindForReading(ID3D11DeviceContext* context);


    private:
        struct GBufferCBuffer
        {
            Mat4 mWVPMatrix;
            Mat4 mWorldMatrix;
            float mTextureTilingFactor;
            int mHasDiffuseTexture;
            int mHasNormalTexture;
            int _padding;

            GBufferCBuffer(const Mat4& wvpMatrix, const Mat4& worldMatrix, const float textureTilingFactor, const bool hasDiffuse, const bool hasNormal) :
                mWVPMatrix(wvpMatrix), mWorldMatrix(worldMatrix), mTextureTilingFactor(textureTilingFactor), mHasDiffuseTexture(hasDiffuse), mHasNormalTexture(hasNormal)
            {
            }
        };


        ID3D11Texture2D* mTextures[DX11GBuffer::GBUFFER_NUM_RENDERTARGETS];
        ID3D11RenderTargetView* mRenderTargets[DX11GBuffer::GBUFFER_NUM_RENDERTARGETS];
        ID3D11ShaderResourceView* mShaderResourceViews[DX11GBuffer::GBUFFER_NUM_RENDERTARGETS];
        ID3D11Texture2D* mDepthStencilBuffer;
        ID3D11DepthStencilView* mDepthStencilView;
        ID3D11DepthStencilState* mDepthStencilState;
        ID3D11InputLayout* mInputLayout;
        ID3D11VertexShader* mVertexShader;
        ID3D11PixelShader* mPixelShader;
        DX11ConstantBuffer<GBufferCBuffer> mConstantBuffer;
    };
}