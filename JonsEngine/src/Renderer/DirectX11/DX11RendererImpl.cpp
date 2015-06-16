#include "include/Renderer/DirectX11/DX11RendererImpl.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/Math/Math.h"
#include "include/Core/Utils/Win32.h"
#include "include/Renderer/DirectX11/DX11Utils.h"

#include <Commctrl.h>
#include <array>


namespace JonsEngine
{
    static DX11RendererImpl* gDX11RendererImpl = nullptr;

    const UINT_PTR gSubClassID = 1;


    DX11Texture::SHADER_TEXTURE_SLOT GetShaderTextureSlot(TextureType textureType)
    {
        switch (textureType)
        {
            case TextureType::TEXTURE_TYPE_DIFFUSE:
                return DX11Texture::SHADER_TEXTURE_SLOT_DIFFUSE;

            case TextureType::TEXTURE_TYPE_NORMAL:
                return DX11Texture::SHADER_TEXTURE_SLOT_NORMAL;

            case TextureType::TEXTURE_TYPE_SKYBOX:
                return DX11Texture::SHADER_TEXTURE_SLOT_EXTRA;

            default:
                {
                    JONS_LOG_ERROR(Logger::GetRendererLogger(), "Bad TextureType provided");
                    throw std::runtime_error("Bad TextureType provided");
                }
        };
    }

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


    DX11RendererImpl::DX11RendererImpl(const EngineSettings& settings, Logger& logger, IMemoryAllocatorPtr memoryAllocator) : 
        DX11Context(GetActiveWindow()), 
        mLogger(logger),
        mMemoryAllocator(memoryAllocator),
        mShadowResolution(settings.mShadowResolution),
        mShadowReadbackLatency(settings.mShadowReadbackLatency),
        mShadowFiltering(settings.mShadowFiltering),
        mAntiAliasing(settings.mAntiAliasing),

        mPipeline(mLogger, mDevice, mSwapchain, mContext, GetBackbufferTextureDesc(), mShadowResolution, mShadowReadbackLatency, mMeshes, mTextures),
        mDepthReductionPass(mDevice, mContext, settings.mShadowReadbackLatency, settings.mWindowWidth, settings.mWindowHeight),

        // samplers
        mModelSampler(mMemoryAllocator->AllocateObject<DX11Sampler>(mDevice, mContext, settings.mAnisotropicFiltering, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, DX11Sampler::SHADER_SAMPLER_SLOT_ANISOTROPIC), [this](DX11Sampler* sampler) { mMemoryAllocator->DeallocateObject(sampler); }),
        mShadowmapSampler(mDevice, mContext, EngineSettings::Anisotropic::X1, D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_COMPARISON_LESS_EQUAL, DX11Sampler::SHADER_SAMPLER_SLOT_POINT_COMPARE),
        mShadowmapNoCompareSampler(mDevice, mContext, EngineSettings::Anisotropic::X1, D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_COMPARISON_ALWAYS, DX11Sampler::SHADER_SAMPLER_SLOT_POINT),

        // misc
        mSSAOEnabled(settings.mSSAOEnabled)
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
    
        DXCALL(mSwapchain->SetFullscreenState(false, nullptr));
    }


    MeshID DX11RendererImpl::CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangentData,
        const std::vector<uint16_t>& indexData, const Vec3& minBounds, const Vec3& maxBounds)
    {
        auto allocator = mMemoryAllocator;

        return mMeshes.AddItem(mDevice, mContext, vertexData, normalData, texCoords, tangentData, indexData, minBounds, maxBounds);
    }

    TextureID DX11RendererImpl::CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight)
    {
        auto allocator = mMemoryAllocator;

        const bool isSRGB = (textureType == TextureType::TEXTURE_TYPE_DIFFUSE);
        const bool isCubemap = (textureType == TextureType::TEXTURE_TYPE_SKYBOX);

        return mTextures.AddItem(mDevice, mContext, textureData, textureWidth, textureHeight, GetShaderTextureSlot(textureType), isCubemap, isSRGB);
    }


    void DX11RendererImpl::Render(const RenderQueue& renderQueue, const DebugOptions::RenderingFlags debugFlags)
    {
        mPipeline.BeginFrame();

        mPipeline.GeometryStage(renderQueue);
        mPipeline.LightingStage(renderQueue, debugFlags, mShadowFiltering, mSSAOEnabled);
        mPipeline.PostProcessingStage(renderQueue, debugFlags, mAntiAliasing);

        mPipeline.EndFrame();
    }

    void DX11RendererImpl::ReduceDepth(const Mat4& cameraProjMatrix, float& minDepth, float& maxDepth)
    {
        mDepthReductionPass.ReduceDepth(cameraProjMatrix, minDepth, maxDepth);
    }


    EngineSettings::Anisotropic DX11RendererImpl::GetAnisotropicFiltering() const
    {
        return mModelSampler->GetMaxAnisotropicFiltering();
    }

    void DX11RendererImpl::SetAnisotropicFiltering(const EngineSettings::Anisotropic anisotropic)
    {
        mModelSampler.reset(mMemoryAllocator->AllocateObject<DX11Sampler>(mDevice, mContext, anisotropic, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, DX11Sampler::SHADER_SAMPLER_SLOT_ANISOTROPIC));
    }


    EngineSettings::AntiAliasing DX11RendererImpl::GetAntiAliasing() const
    {
        return mAntiAliasing;
    }

    void DX11RendererImpl::SetAntiAliasing(const EngineSettings::AntiAliasing aa)
    {
        mAntiAliasing = aa;
    }


	EngineSettings::ShadowFiltering DX11RendererImpl::GetShadowFiltering() const
	{
		return mShadowFiltering;
	}

	void DX11RendererImpl::SetShadowFiltering(const EngineSettings::ShadowFiltering shadowFiltering)
	{
		mShadowFiltering = shadowFiltering;
	}


    bool DX11RendererImpl::IsSSAOEnabled() const
    {
        return mSSAOEnabled;
    }

    void DX11RendererImpl::SetSSAO(const bool useSSAO)
    {
        mSSAOEnabled = useSSAO;
    }


    float DX11RendererImpl::GetZNear() const
    {
        return Z_NEAR;
    }

    float DX11RendererImpl::GetZFar() const
    {
        return Z_FAR;
    }

    EngineSettings::ShadowResolution DX11RendererImpl::GetShadowResolution() const
    {
        return mShadowResolution;
    }

    EngineSettings::ShadowReadbackLatency DX11RendererImpl::GetShadowReadbackLatency() const
    {
        return mShadowReadbackLatency;
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
        mShadowmapSampler.BindSampler();
        mShadowmapNoCompareSampler.BindSampler();
    }
}