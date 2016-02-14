#pragma once

#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Types.h"
#include "include/Core/Platform/Directx11.h"

#include <array>

namespace JonsEngine
{
    class DX11GBuffer
    {
    public:
        enum GBUFFER_RENDERTARGET_INDEX
        {
            GBUFFER_RENDERTARGET_INDEX_DIFFUSE,
            GBUFFER_RENDERTARGET_INDEX_NORMAL,
            GBUFFER_NUM_RENDERTARGETS,
        };


        DX11GBuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, D3D11_TEXTURE2D_DESC backbufferTextureDesc);
        ~DX11GBuffer();

        void SetConstantData(const Mat4& wvpMatrix, const Mat4& worldMatrix, const float textureTilingFactor, const bool hasDiffuseTexture, const bool hasNormalTexture, const bool isAnimating);
        void BindForGeometryStage(ID3D11DepthStencilViewPtr dsv);
        void BindGeometryTextures();


    private:
        struct GBufferCBuffer
        {
            Mat4 mWVPMatrix;
            Mat4 mWorldViewMatrix;
            float mTextureTilingFactor;
            uint32_t mHasDiffuseTexture;
            uint32_t mHasNormalTexture;
            uint32_t mIsAnimating;

            GBufferCBuffer(const Mat4& wvpMatrix, const Mat4& worldViewMatrix, const float textureTilingFactor, const bool hasDiffuse, const bool hasNormal, const bool isAnimating) :
                mWVPMatrix(wvpMatrix),
                mWorldViewMatrix(worldViewMatrix),
                mTextureTilingFactor(textureTilingFactor),
                mHasDiffuseTexture(hasDiffuse),
                mHasNormalTexture(hasNormal),
                mIsAnimating(isAnimating)
            {
            }
        };

        std::array<ID3D11Texture2DPtr, DX11GBuffer::GBUFFER_NUM_RENDERTARGETS> mTextures;
        std::array<ID3D11RenderTargetViewPtr, DX11GBuffer::GBUFFER_NUM_RENDERTARGETS> mRenderTargets;
        std::array<ID3D11ShaderResourceViewPtr, DX11GBuffer::GBUFFER_NUM_RENDERTARGETS> mShaderResourceViews;
        ID3D11DeviceContextPtr mContext;
        ID3D11InputLayoutPtr mInputLayout;
        ID3D11VertexShaderPtr mVertexShader;
        ID3D11PixelShaderPtr mPixelShader;
        DX11ConstantBuffer<GBufferCBuffer> mConstantBuffer;
    };
}
