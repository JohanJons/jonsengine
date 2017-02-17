#include "include/Renderer/DirectX11/DX11RendererImpl.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/RenderSettings.h"
#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/Math/Math.h"

#include <Commctrl.h>
#include <array>


namespace JonsEngine
{
    static DX11RendererImpl* gDX11RendererImpl = nullptr;

    const UINT_PTR gSubClassID = 1;

    DXGI_FORMAT GetTextureFormat(const TextureType textureType);


    LRESULT CALLBACK DX11RendererImpl::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
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

                if (wParam)     // --> to fullscreen
                {
                    DXCALL(gDX11RendererImpl->mSwapchain->ResizeTarget(&displayDesc));
                    DXCALL(gDX11RendererImpl->mSwapchain->SetFullscreenState(wParam, NULL));
                }
                else
                {
                    DXCALL(gDX11RendererImpl->mSwapchain->SetFullscreenState(wParam, NULL));
                    DXCALL(gDX11RendererImpl->mSwapchain->ResizeTarget(&displayDesc));
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

                DXCALL(gDX11RendererImpl->mSwapchain->ResizeTarget(&displayDesc));

				return 0;
			}

            case WM_SIZE:
            {
                const uint16_t width = LOWORD(lParam);
                const uint16_t height = HIWORD(lParam);

               /* gDX11RendererImpl->mContext->ClearState();
                gDX11RendererImpl->mContext->OMSetRenderTargets(0, 0, 0);
                gDX11RendererImpl->mBackbuffer->Release();
                gDX11RendererImpl->mBackbuffer = nullptr;*/
                // TODO

               // DXCALL(gDX11RendererImpl->mSwapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));

                // backbuffer rendertarget setup
                //ID3D11Texture2DPtr backbuffer = nullptr;
                //DXCALL(gDX11RendererImpl->mSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer));

                //DXCALL(gDX11RendererImpl->mDevice->CreateRenderTargetView(backbuffer, NULL, &gDX11RendererImpl->mBackbuffer));
                //backbuffer->Release();

               // gDX11RendererImpl->SetupContext(width, height);

                return 0;
            }
            
            default:
                return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }
    }


    DX11RendererImpl::DX11RendererImpl(const RenderSettings& renderSettings, const WindowSettings& windowSettings, Logger& logger, HeapAllocator& memoryAllocator) : 
        DX11Context(GetActiveWindow()), 
        mLogger(logger),
        mMemoryAllocator(memoryAllocator),
		mRenderSettings(renderSettings),

		mBackbuffer(mDevice, mContext, mSwapchain),

		mDepthReadback(mDevice, mContext, mBackbuffer.GetDepthbuffer()),
		mDepthReductionPass(mDevice, mContext, mRenderSettings.mShadowReadbackLatency, windowSettings.mWindowDimensions.x, windowSettings.mWindowDimensions.y),

        // samplers
        mModelSampler(mMemoryAllocator.AllocateObject<DX11Sampler>(mDevice, mContext, mRenderSettings.mAnisotropicFiltering, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, DX11Sampler::SHADER_SAMPLER_SLOT_ANISOTROPIC), [this](DX11Sampler* sampler) { mMemoryAllocator.DeallocateObject(sampler); }),
        mLinearSampler(mDevice, mContext, RenderSettings::Anisotropic::X1, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, DX11Sampler::SHADER_SAMPLER_SLOT_LINEAR),
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

        gDX11RendererImpl = this;
    }

    DX11RendererImpl::~DX11RendererImpl()
    {
        RemoveWindowSubclass(mWindowHandle, WndProc, gSubClassID);
    
		// TODO: might throw exception!
        DXCALL(mSwapchain->SetFullscreenState(false, nullptr));
    }


    DX11MeshID DX11RendererImpl::CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangentData,
        const std::vector<uint16_t>& indexData, const Vec3& minBounds, const Vec3& maxBounds)
    {
        return mMeshes.Insert(mDevice, mContext, vertexData, normalData, texCoords, tangentData, indexData, minBounds, maxBounds);
    }

    DX11MeshID DX11RendererImpl::CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangentData,
		const std::vector<BoneWeight>& boneWeights, const std::vector<uint16_t>& indexData, const Vec3& minBounds, const Vec3& maxBounds)
    {
        return mMeshes.Insert(mDevice, mContext, vertexData, normalData, texCoords, tangentData, boneWeights, indexData, minBounds, maxBounds);
    }

    DX11TextureID DX11RendererImpl::CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight)
    {
        const bool isCubemap = textureType == TextureType::Skybox;

        return mTextures.Insert(mDevice, mContext, textureData, GetTextureFormat(textureType), textureWidth, textureHeight, isCubemap);
    }


    void DX11RendererImpl::Render(const RenderQueue& renderQueue, const Milliseconds elapstedFrameTime, const DebugOptions::RenderingFlags debugFlags)
    {
        mPipeline.BeginFrame(renderQueue);

        mPipeline.GeometryStage(renderQueue);
        mPipeline.LightingStage(renderQueue, debugFlags, mRenderSettings);
        mPipeline.PostProcessingStage(renderQueue, elapstedFrameTime, debugFlags, mRenderSettings);

        mPipeline.EndFrame();
    }

    void DX11RendererImpl::ReduceDepth(float& minDepth, float& maxDepth)
    {
        mDepthReductionPass.ReduceDepth(minDepth, maxDepth);
    }


	RenderSettings::Anisotropic DX11RendererImpl::GetAnisotropicFiltering() const
    {
        return mModelSampler->GetMaxAnisotropicFiltering();
    }

    void DX11RendererImpl::SetAnisotropicFiltering(const RenderSettings::Anisotropic anisotropic)
    {
        mModelSampler.reset(mMemoryAllocator.AllocateObject<DX11Sampler>(mDevice, mContext, anisotropic, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, DX11Sampler::SHADER_SAMPLER_SLOT_ANISOTROPIC));
    }


    RenderSettings::AntiAliasing DX11RendererImpl::GetAntiAliasing() const
    {
        return mRenderSettings.mAntiAliasing;
    }

    void DX11RendererImpl::SetAntiAliasing(const RenderSettings::AntiAliasing aa)
    {
		mRenderSettings.mAntiAliasing = aa;
    }


	RenderSettings::ShadowFiltering DX11RendererImpl::GetShadowFiltering() const
	{
		return mRenderSettings.mShadowFiltering;
	}

	void DX11RendererImpl::SetShadowFiltering(const RenderSettings::ShadowFiltering shadowFiltering)
	{
		mRenderSettings.mShadowFiltering = shadowFiltering;
	}


	RenderSettings::ToneMappingAlghorithm DX11RendererImpl::GetToneMappingAlghorithm() const
	{
		return mRenderSettings.mToneMapping;
	}

	void DX11RendererImpl::SetToneMappingAlghorithm(const RenderSettings::ToneMappingAlghorithm alghorithm)
	{
		mRenderSettings.mToneMapping = alghorithm;
	}


    bool DX11RendererImpl::IsSSAOEnabled() const
    {
        return mSSAOEnabled;
    }

    void DX11RendererImpl::SetSSAO(const bool useSSAO)
    {
        mSSAOEnabled = useSSAO;
    }


	float DX11RendererImpl::GetDepthValue(const WindowPosition& position)
	{
		return mDepthReadback.GetDepthValue(position);
	}

    float DX11RendererImpl::GetZNear() const
    {
        return Z_NEAR;
    }

    float DX11RendererImpl::GetZFar() const
    {
        return Z_FAR;
    }

    RenderSettings::ShadowResolution DX11RendererImpl::GetShadowResolution() const
    {
        return mRenderSettings.mShadowResolution;
    }

    RenderSettings::ShadowReadbackLatency DX11RendererImpl::GetShadowReadbackLatency() const
    {
        return mRenderSettings.mShadowReadbackLatency;
    }


    void DX11RendererImpl::SetupContext(const uint32_t viewportWidth, const uint32_t viewportHeight)
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
        mShadowmapSampler.BindSampler();
        mShadowmapNoCompareSampler.BindSampler();
    }


    DXGI_FORMAT GetTextureFormat(const TextureType textureType)
    {
        switch (textureType)
        {
            case TextureType::Diffuse:
            case TextureType::Skybox:
			case TextureType::Height:
                return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

            case TextureType::Normal:
                return DXGI_FORMAT_R8G8B8A8_UNORM;

            default:
            {
                JONS_LOG_ERROR(Logger::GetRendererLogger(), "Bad TextureType provided");
                throw std::runtime_error("Bad TextureType provided");
            }
        }
    }
}