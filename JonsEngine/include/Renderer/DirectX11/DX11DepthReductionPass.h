#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11RenderTarget2D.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Core/Types.h"
#include "include/Core/EngineSettings.h"

#include <array>
#include <vector>

namespace JonsEngine
{
    class DX11DepthReductionPass
    {
    public:
        const static uint32_t MAX_READBACK_LATENCY = 3;

        DX11DepthReductionPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const EngineSettings::ShadowReadbackLatency readbackLatency, const uint16_t windowWidth, const uint16_t windowHeight);
        ~DX11DepthReductionPass();


        void ReduceDepth(const Mat4& cameraProjMatrix, float& minDepth, float& maxDepth);


    private:
        struct SDSMCBuffer
        {
            float mProjection33;
            float mProjection43;
            float mNearClip;
            float mFarClip;


            inline SDSMCBuffer(const float proj33, const float proj43, const float nearClip, const float farClip) :
                mProjection33(proj33), mProjection43(proj43), mNearClip(nearClip), mFarClip(farClip)
            {
            }
        };

        const uint32_t mReadbackLatency;
        uint32_t mCurrFrame;

        ID3D11DeviceContextPtr mContext;

        ID3D11ComputeShaderPtr mSDSMInitialShader;
        ID3D11ComputeShaderPtr mSDSMFinalShader;

        std::array<ID3D11Texture2DPtr, MAX_READBACK_LATENCY> mReadbackTextures;
        std::vector<DX11RenderTarget2D> mDepthReductionRTVs;

        DX11ConstantBuffer<SDSMCBuffer> mSDSMCBuffer;
    };
}