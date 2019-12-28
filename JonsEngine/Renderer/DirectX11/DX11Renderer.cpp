#include "Renderer/DirectX11/DX11Renderer.h"

#include "Renderer/DirectX11/DX11Utils.h"
#include "Renderer/DirectX11/PerlinNoise.hpp"
#include "Renderer/RenderSettings.h"
#include "Core/Logging/Logger.h"
#include "Core/Memory/HeapAllocator.h"
#include "Core/Math/MathUtils.h"

#include <Commctrl.h>
#include <array>


namespace JonsEngine
{
    static DX11Renderer* gDX11Renderer = nullptr;

    const UINT_PTR gSubClassID = 1;


    LRESULT CALLBACK DX11Renderer::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
    {
        switch (uMsg)
        {
            case WM_JONS_FULLSCREEN:
            {
                const uint16_t width = LOWORD(lParam);
                const uint16_t height = HIWORD(lParam);

                DXGI_MODE_DESC displayDesc;
                ZeroMemory(&displayDesc, sizeof(D3D11_VIEWPORT));
                displayDesc.Width = width;
                displayDesc.Height = height;
                displayDesc.Format = DXGI_FORMAT_UNKNOWN;

                bool doFullscreen = static_cast<bool>( wParam );
                if (doFullscreen)     // --> to fullscreen
                {
                    DXCALL(gDX11Renderer->mSwapchain->ResizeTarget(&displayDesc));
                    DXCALL(gDX11Renderer->mSwapchain->SetFullscreenState(doFullscreen, NULL));
                }
                else
                {
                    DXCALL(gDX11Renderer->mSwapchain->SetFullscreenState(doFullscreen, NULL));
                    DXCALL(gDX11Renderer->mSwapchain->ResizeTarget(&displayDesc));
                }

                return 0;
            }

            case WM_JONS_RESIZE:
            {
                const uint16_t width = LOWORD(lParam);
                const uint16_t height = HIWORD(lParam);

                DXGI_MODE_DESC displayDesc;
                ZeroMemory(&displayDesc, sizeof(D3D11_VIEWPORT));
                displayDesc.Width = width;
                displayDesc.Height = height;
                displayDesc.Format = DXGI_FORMAT_UNKNOWN;

                DXCALL(gDX11Renderer->mSwapchain->ResizeTarget(&displayDesc));

                return 0;
            }

            case WM_SIZE:
            {
                const uint16_t width = LOWORD(lParam);
                const uint16_t height = HIWORD(lParam);

                /* gDX11Renderer->mContext->ClearState();
                gDX11Renderer->mContext->OMSetRenderTargets(0, 0, 0);
                gDX11Renderer->mBackbuffer->Release();
                gDX11Renderer->mBackbuffer = nullptr;*/
                // TODO

                // DXCALL(gDX11Renderer->mSwapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));

                // backbuffer rendertarget setup
                //ID3D11Texture2DPtr backbuffer = nullptr;
                //DXCALL(gDX11Renderer->mSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer));

                //DXCALL(gDX11Renderer->mDevice->CreateRenderTargetView(backbuffer, NULL, &gDX11Renderer->mBackbuffer));
                //backbuffer->Release();

                // gDX11Renderer->SetupContext(width, height);

                return 0;
            }

            default:
                return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }
    }


    DX11Renderer::DX11Renderer(const RenderSettings& renderSettings, const WindowSettings& windowSettings, HeapAllocator& memoryAllocator, Logger& logger) : 
        DX11Context(GetActiveWindow()), 
        mLogger(logger),
        mMemoryAllocator(memoryAllocator),
        mRenderSettings(renderSettings),

        mBackbuffer(mDevice, mContext, mSwapchain),

        mDepthReadback(mDevice, mContext, mBackbuffer.GetDepthbuffer()),
        mDepthReductionPass(mDevice, mContext, mRenderSettings.mShadowReadbackLatency, windowSettings.mWindowDimensions.x, windowSettings.mWindowDimensions.y),

        // samplers
        // note: need to re-evaluate the W-component *_WRAP on linear sampler. Something that was done with tonemapping...
        // note2: revisit and refactor whole sampler part...
        mModelSampler(mMemoryAllocator.AllocateObject<DX11Sampler>(mDevice, mContext, mRenderSettings.mAnisotropicFiltering, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, DX11Sampler::SHADER_SAMPLER_SLOT_ANISOTROPIC), [this](DX11Sampler* sampler) { mMemoryAllocator.DeallocateObject(sampler); }),
        mLinearSampler(mDevice, mContext, RenderSettings::Anisotropic::X1, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, DX11Sampler::SHADER_SAMPLER_SLOT_LINEAR),
        mLinearWrapSampler( mDevice, mContext, RenderSettings::Anisotropic::X1, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, DX11Sampler::SHADER_SAMPLER_SLOT_LINEAR_WRAP ),
        mShadowmapSampler(mDevice, mContext, RenderSettings::Anisotropic::X1, D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_COMPARISON_LESS_EQUAL, DX11Sampler::SHADER_SAMPLER_SLOT_POINT_COMPARE),
        mShadowmapNoCompareSampler(mDevice, mContext, RenderSettings::Anisotropic::X1, D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_COMPARISON_ALWAYS, DX11Sampler::SHADER_SAMPLER_SLOT_POINT),

        // misc
        mSSAOEnabled(mRenderSettings.mSSAOEnabled),

        mPipeline(mLogger, mDevice, mSwapchain, mContext, GetBackbufferTextureDesc(), mBackbuffer, mRenderSettings, mMeshes, mTextures)
    {
        // set CCW as front face
        D3D11_RASTERIZER_DESC rasterizerDesc;
        ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.CullMode = D3D11_CULL_BACK;
        rasterizerDesc.FrontCounterClockwise = true;
        rasterizerDesc.DepthClipEnable = true;
        rasterizerDesc.ScissorEnable = false;
        rasterizerDesc.MultisampleEnable = false;
        rasterizerDesc.AntialiasedLineEnable = false;
        DXCALL(mDevice->CreateRasterizerState(&rasterizerDesc, &mDefaultRasterizerState));

        SetupContext(mSwapchainDesc.BufferDesc.Width, mSwapchainDesc.BufferDesc.Height);

        // register as window subclass to listen for WM_SIZE events. etc
        if (!SetWindowSubclass(mWindowHandle, WndProc, gSubClassID, 0))
        {
            JONS_LOG_ERROR(mLogger, "DX11Renderer::DX11Renderer(): SetWindowSubclass() failed");
            throw std::runtime_error("DX11Renderer::DX11Renderer(): SetWindowSubclass() failed");
        }

        // general usage perlin-noise texture exposed to shaders
        std::vector<uint8_t> perlinNoiseVec( gPerlinNoiseArr, gPerlinNoiseArr + ( gPerlinNoiseArrWidth * gPerlinNoiseArrHeight ) );
        DX11TextureID perlinNoiseID = CreateTexture( TextureType::Height, perlinNoiseVec, gPerlinNoiseArrWidth, gPerlinNoiseArrHeight );
        mTextures.GetItem( perlinNoiseID ).BindAsShaderResource( SHADER_TEXTURE_SLOT_PERLIN );

        gDX11Renderer = this;
    }

    DX11Renderer::~DX11Renderer()
    {
        RemoveWindowSubclass(mWindowHandle, WndProc, gSubClassID);

        // TODO: might throw exception!
        DXCALL(mSwapchain->SetFullscreenState(false, nullptr));
    }


    DX11MeshID DX11Renderer::CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangentData,
        const std::vector<uint16_t>& indexData, const Vec3& minBounds, const Vec3& maxBounds)
    {
        return mMeshes.Insert(mDevice, mContext, vertexData, normalData, texCoords, tangentData, indexData, minBounds, maxBounds);
    }

    DX11MeshID DX11Renderer::CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangentData,
        const std::vector<BoneWeight>& boneWeights, const std::vector<uint16_t>& indexData, const Vec3& minBounds, const Vec3& maxBounds)
    {
        return mMeshes.Insert(mDevice, mContext, vertexData, normalData, texCoords, tangentData, boneWeights, indexData, minBounds, maxBounds);
    }

    DX11TextureID DX11Renderer::CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight)
    {
        return mTextures.Insert( mDevice, mContext, textureData, textureType, textureWidth, textureHeight );
    }


    void DX11Renderer::Render(const RenderQueue& renderQueue, const Milliseconds elapstedFrameTime, const DebugOptions::RenderingFlags debugFlags)
    {
        mPipeline.BeginFrame( renderQueue, mRenderSettings );

        mPipeline.GeometryStage( renderQueue, mRenderSettings );
        mPipeline.LightingStage( renderQueue, mRenderSettings );
        mPipeline.PostProcessingStage( renderQueue, elapstedFrameTime, mRenderSettings );
        mPipeline.DebugStage( renderQueue, mRenderSettings, debugFlags );

        mPipeline.EndFrame( mRenderSettings );
    }

    void DX11Renderer::ReduceDepth(float& minDepth, float& maxDepth)
    {
        mDepthReductionPass.ReduceDepth(minDepth, maxDepth);
    }


    RenderSettings::Anisotropic DX11Renderer::GetAnisotropicFiltering() const
    {
        return mModelSampler->GetMaxAnisotropicFiltering();
    }

    void DX11Renderer::SetAnisotropicFiltering(const RenderSettings::Anisotropic anisotropic)
    {
        mModelSampler.reset(mMemoryAllocator.AllocateObject<DX11Sampler>(mDevice, mContext, anisotropic, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, DX11Sampler::SHADER_SAMPLER_SLOT_ANISOTROPIC));
    }


    RenderSettings::AntiAliasing DX11Renderer::GetAntiAliasing() const
    {
        return mRenderSettings.mAntiAliasing;
    }

    void DX11Renderer::SetAntiAliasing(const RenderSettings::AntiAliasing aa)
    {
        mRenderSettings.mAntiAliasing = aa;
    }


    RenderSettings::ShadowFiltering DX11Renderer::GetShadowFiltering() const
    {
        return mRenderSettings.mShadowFiltering;
    }

    void DX11Renderer::SetShadowFiltering(const RenderSettings::ShadowFiltering shadowFiltering)
    {
        mRenderSettings.mShadowFiltering = shadowFiltering;
    }


    RenderSettings::ToneMappingAlghorithm DX11Renderer::GetToneMappingAlghorithm() const
    {
        return mRenderSettings.mToneMapping;
    }

    void DX11Renderer::SetToneMappingAlghorithm(const RenderSettings::ToneMappingAlghorithm alghorithm)
    {
        mRenderSettings.mToneMapping = alghorithm;
    }

	void DX11Renderer::SetTerrainPatchSize( RenderSettings::TerrainPatchSize patchSize )
	{
		mRenderSettings.mTerrainPatchSize = patchSize;
	}

	void DX11Renderer::SetTerrainPatchVertexRatio( RenderSettings::TerrainPatchVerticeRatio vertexRatio )
	{
		mRenderSettings.mTerrainVertexRatio = vertexRatio;
	}

    bool DX11Renderer::IsSSAOEnabled() const
    {
        return mSSAOEnabled;
    }

    void DX11Renderer::SetSSAO(const bool useSSAO)
    {
        mSSAOEnabled = useSSAO;
    }


    float DX11Renderer::GetDepthValue(const WindowPosition& position)
    {
        return mDepthReadback.GetDepthValue(position);
    }

    float DX11Renderer::GetZNear() const
    {
        return Z_NEAR;
    }

    float DX11Renderer::GetZFar() const
    {
        return Z_FAR;
    }

    RenderSettings::ShadowResolution DX11Renderer::GetShadowResolution() const
    {
        return mRenderSettings.mShadowResolution;
    }

    RenderSettings::ShadowReadbackLatency DX11Renderer::GetShadowReadbackLatency() const
    {
        return mRenderSettings.mShadowReadbackLatency;
    }


    void DX11Renderer::SetupContext(const uint32_t viewportWidth, const uint32_t viewportHeight)
    {
        mContext->RSSetState(mDefaultRasterizerState);

        D3D11_VIEWPORT viewport;
        ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = static_cast<float>(viewportWidth);
        viewport.Height = static_cast<float>(viewportHeight);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        mContext->RSSetViewports(1, &viewport);

        mModelSampler->BindSampler();
        mLinearSampler.BindSampler();
        mLinearWrapSampler.BindSampler();
        mShadowmapSampler.BindSampler();
        mShadowmapNoCompareSampler.BindSampler();
    }
}