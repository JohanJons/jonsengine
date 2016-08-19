#include "include/Renderer/DirectX11/DX11DepthReductionPass.h"

#include "include/Renderer/DirectX11/Shaders/Compiled/SDSMInitialCompute.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/SDSMFinalCompute.h"
#include "include/Core/Math/Math.h"

namespace JonsEngine
{
    // Computes a compute shader dispatch size given a thread group size, and number of elements to process
    uint32_t DispatchSize(uint32_t tgSize, uint32_t numElements)
    {
        uint32_t dispatchSize = numElements / tgSize;
        dispatchSize += numElements % tgSize > 0 ? 1 : 0;

        return dispatchSize;
    }


    DX11DepthReductionPass::DX11DepthReductionPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const EngineSettings::ShadowReadbackLatency readbackLatency, const uint16_t windowWidth, const uint16_t windowHeight) :
        mReadbackLatency(EngineSettingsToVal(readbackLatency)), mCurrFrame(0), mContext(context), mSDSMInitialShader(nullptr), mSDSMFinalShader(nullptr), mSDSMCBuffer(device, context, mSDSMCBuffer.CONSTANT_BUFFER_SLOT_COMPUTE)
    {
        DXCALL(device->CreateComputeShader(gSDSMInitialComputeShader, sizeof(gSDSMInitialComputeShader), nullptr, &mSDSMInitialShader));
        DXCALL(device->CreateComputeShader(gSDSMFinalComputeShader, sizeof(gSDSMFinalComputeShader), nullptr, &mSDSMFinalShader));

        // setup UAVs for compute shader
        uint32_t width = windowWidth;
        uint32_t height = windowHeight;
        while (width > 1 || height > 1)
        {
            width = DispatchSize(SDSM_THREAD_GROUP_SIZE, width);
            height = DispatchSize(SDSM_THREAD_GROUP_SIZE, height);

            mDepthReductionRTVs.emplace_back(device, DXGI_FORMAT_R16G16_UNORM, width, height, true);
        }

        // setup readback textures
        D3D11_TEXTURE2D_DESC textureDesc;
        ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
        textureDesc.Width = 1;
        textureDesc.Height = 1;
        textureDesc.ArraySize = 1;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Format = DXGI_FORMAT_R16G16_UNORM;
        textureDesc.Usage = D3D11_USAGE_STAGING;
        textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        for (uint32_t index = 0; index < MAX_READBACK_LATENCY; ++index)
            DXCALL(device->CreateTexture2D(&textureDesc, nullptr, &mReadbackTextures[index]));
    }

    DX11DepthReductionPass::~DX11DepthReductionPass()
    {
    }


    void DX11DepthReductionPass::ReduceDepth(float& minDepth, float& maxDepth)
    {
        mContext->OMSetRenderTargets(0, nullptr, nullptr);

        // first pass
        // TODO: why neg. [2].z?
        const float perspClipNear = PerspectiveNearPlane(Z_NEAR, Z_FAR);
        const float perspClipFar = PerspectiveFarPlane(Z_NEAR, Z_FAR);
        mSDSMCBuffer.SetData(SDSMCBuffer(-perspClipNear, perspClipFar, Z_NEAR, Z_FAR));

        auto& initialRTV = mDepthReductionRTVs.front();
        mContext->CSSetUnorderedAccessViews(UAV_SLOT, 1, &initialRTV.mUAV.p, nullptr);
        mContext->CSSetShader(mSDSMInitialShader, nullptr, 0);

        D3D11_TEXTURE2D_DESC rtvTextureDesc;
        ZeroMemory(&rtvTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
        initialRTV.mTexture->GetDesc(&rtvTextureDesc);

        mContext->Dispatch(rtvTextureDesc.Width, rtvTextureDesc.Height, 1);

        mContext->CSSetUnorderedAccessViews(UAV_SLOT, 1, &gNullUAV.p, nullptr);

        // subsequent passes
        mContext->CSSetShader(mSDSMFinalShader, nullptr, 0);
        for (uint32_t index = 1; index < mDepthReductionRTVs.size(); ++index)
        {
            auto& prevRTV = mDepthReductionRTVs.at(index - 1);
            mContext->CSSetShaderResources(SHADER_TEXTURE_SLOT_EXTRA, 1, &prevRTV.mSRV.p);

            auto& rtv = mDepthReductionRTVs.at(index);
            mContext->CSSetUnorderedAccessViews(UAV_SLOT, 1, &rtv.mUAV.p, nullptr);

            ZeroMemory(&rtvTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
            rtv.mTexture->GetDesc(&rtvTextureDesc);

            mContext->Dispatch(rtvTextureDesc.Width, rtvTextureDesc.Height, 1);

            mContext->CSSetUnorderedAccessViews(UAV_SLOT, 1, &gNullUAV.p, nullptr);
            mContext->CSSetShaderResources(SHADER_TEXTURE_SLOT_EXTRA, 1, &gNullSRV.p);
        }

        // reading back depth
        ID3D11Texture2DPtr lastTarget = mDepthReductionRTVs[mDepthReductionRTVs.size() - 1].mTexture;
        mContext->CopyResource(mReadbackTextures[mCurrFrame % mReadbackLatency].p, lastTarget.p);

        // note: when this overflows, will cause a few bad frames
        ++mCurrFrame;
        if (mCurrFrame >= mReadbackLatency)
        {
            ID3D11Texture2DPtr readbackTexture = mReadbackTextures[mCurrFrame % mReadbackLatency];

            D3D11_MAPPED_SUBRESOURCE mapped;
            DXCALL(mContext->Map(readbackTexture.p, 0, D3D11_MAP_READ, 0, &mapped));
            mContext->Unmap(readbackTexture.p, 0);

            const uint16_t* texData = reinterpret_cast<uint16_t*>(mapped.pData);

            minDepth = texData[0] / static_cast<float>(0xffff);
            maxDepth = texData[1] / static_cast<float>(0xffff);
        }
        else
        {
            minDepth = 0.01f;
            maxDepth = 1.0f;
        }
    }
}