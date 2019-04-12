#pragma once

#include "DX11ConstantBuffer.hpp"
#include "DX11Utils.h"
#include "Types.h"
#include "Platform.h"

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

        void SetConstantData(const Mat4& wvpMatrix, const Mat4& worldMatrix, const float textureTilingFactor, const bool hasDiffuseTexture, const bool hasNormalTexture, const uint32_t boneIndexOffset);
        void BindForGeometryStage(ID3D11DepthStencilViewPtr dsv);
		void BindForRendering();
		void BindForStaticPass();
		void BindForAnimatedPass();
        void BindGeometryTextures();


    private:
        struct GBufferCBuffer
        {
            Mat4 mWVPMatrix;
            Mat4 mWorldViewMatrix;
            float mTextureTilingFactor;
            uint32_t mHasDiffuseTexture;
            uint32_t mHasNormalTexture;
			uint32_t mBoneIndexOffset;

            GBufferCBuffer(const Mat4& wvpMatrix, const Mat4& worldViewMatrix, const float textureTilingFactor, const bool hasDiffuse, const bool hasNormal, const uint32_t boneIndexOffset) :
                mWVPMatrix(wvpMatrix),
                mWorldViewMatrix(worldViewMatrix),
                mTextureTilingFactor(textureTilingFactor),
                mHasDiffuseTexture(hasDiffuse),
                mHasNormalTexture(hasNormal),
				mBoneIndexOffset(boneIndexOffset)
            {
            }
        };

        std::array<ID3D11Texture2DPtr, DX11GBuffer::GBUFFER_NUM_RENDERTARGETS> mTextures;
        std::array<ID3D11RenderTargetViewPtr, DX11GBuffer::GBUFFER_NUM_RENDERTARGETS> mRenderTargets;
        std::array<ID3D11ShaderResourceViewPtr, DX11GBuffer::GBUFFER_NUM_RENDERTARGETS> mShaderResourceViews;
        ID3D11DeviceContextPtr mContext;

        ID3D11InputLayoutPtr mStaticLayout;
		ID3D11InputLayoutPtr mAnimatedLayout;
        ID3D11VertexShaderPtr mStaticVertexShader;
		ID3D11VertexShaderPtr mAnimatedVertexShader;

        ID3D11PixelShaderPtr mPixelShader;
        DX11ConstantBuffer<GBufferCBuffer> mConstantBuffer;
    };
}
