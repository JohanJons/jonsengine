#pragma once

#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11Shadowmap.h"
#include "include/Renderer/DirectX11/DX11RenderTarget2D.h"
#include "include/Core/Types.h"
#include "include/Core/EngineSettings.h"

#include <d3d11.h>
#include <array>
#include <vector>

namespace JonsEngine
{
    class DX11FullscreenTrianglePass;
    class DX11VertexTransformPass;
	struct RenderableDirLight;

    class DX11DirectionalLightPass
    {
    public:
        const static uint32_t NUM_SHADOWMAP_CASCADES = 4;
        const static uint32_t MAX_READBACK_LATENCY = 3;

        DX11DirectionalLightPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11FullscreenTrianglePass& fullscreenPass, DX11VertexTransformPass& transformPass, const EngineSettings::ShadowResolution shadowmapRes,
            const EngineSettings::ShadowReadbackLatency readbackLatency, const uint32_t windowWidth, const uint32_t windowHeight);
        ~DX11DirectionalLightPass();

        void Render(const RenderableDirLight& directionalLight, const EngineSettings::ShadowFiltering shadowFiltering, const float degreesFOV, const float aspectRatio, const Mat4& cameraViewMatrix, const Mat4& invCameraProjMatrix, const Vec2& windowSize, const Mat4& cameraProjMatrix);


    private:
        struct DirectionalLightCBuffer
        {
            std::array<Mat4, NUM_SHADOWMAP_CASCADES> mSplitVPMatrices;
            Mat4 mInvProjMatrix;
            std::array<float, NUM_SHADOWMAP_CASCADES> mSplitDistances;
            Vec4 mLightColor;
            Vec4 mLightDirection;
            Vec2 mWindowSize;
            float mShadowmapSize;
            float __padding;


            DirectionalLightCBuffer(const std::array<Mat4, NUM_SHADOWMAP_CASCADES>& splitMatrices, const Mat4& invProjMatrix, const std::array<float, NUM_SHADOWMAP_CASCADES>& splitDistances, const Vec4& lightColor, const Vec4& lightDir, const Vec2& windowSize, const float shadowmapSize) :
                mSplitVPMatrices(splitMatrices), mInvProjMatrix(invProjMatrix), mSplitDistances(splitDistances), mLightColor(lightColor), mLightDirection(lightDir), mWindowSize(windowSize), mShadowmapSize(shadowmapSize)
            {
            }
        };

        struct SDSMCBuffer
        {
            float mProjection33;
            float mProjection43;
            float mNearClip;
            float mFarClip;


            SDSMCBuffer(const float proj33, const float proj43, const float nearClip, const float farClip) :
                mProjection33(proj33), mProjection43(proj43), mNearClip(nearClip), mFarClip(farClip)
            {
            }
        };

		float CalculateShadowmapCascades(const Mat4& cameraProjMatrix, std::array<float, NUM_SHADOWMAP_CASCADES>& splitDistances);
        void ReduceDepth(const Mat4& cameraProjMatrix, float& minDepth, float& maxDepth);
        void BindShadingPixelShader(const EngineSettings::ShadowFiltering shadowFiltering);

        const EngineSettings::ShadowReadbackLatency mReadbackLatency;
        uint32_t mCurrFrame;

        ID3D11DeviceContextPtr mContext;
        std::vector<DX11RenderTarget2D> mDepthReductionRTVs;
        std::array<ID3D11Texture2DPtr, MAX_READBACK_LATENCY> mReadbackTextures;

        ID3D11ComputeShaderPtr mSDSMInitialShader;
        ID3D11ComputeShaderPtr mSDSMFinalShader;
        ID3D11PixelShaderPtr mPCF2x2Shader;
        ID3D11PixelShaderPtr mPCF3x3Shader;
        ID3D11PixelShaderPtr mPCF5x5Shader;
        ID3D11PixelShaderPtr mPCF7x7Shader;

        ID3D11RasterizerStatePtr mRSDepthClamp;
        DX11FullscreenTrianglePass& mFullscreenPass;
        DX11VertexTransformPass& mVertexTransformPass;
        DX11Shadowmap mShadowmap;
        DX11ConstantBuffer<DirectionalLightCBuffer> mDirLightCBuffer;
        DX11ConstantBuffer<SDSMCBuffer> mSDSMCBuffer;
    };
}
