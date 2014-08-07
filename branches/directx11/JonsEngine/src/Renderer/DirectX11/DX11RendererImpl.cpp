#include "include/Renderer/DirectX11/DX11RendererImpl.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/Utils/Math.h"
#include "include/Core/Utils/Win32.h"
#include "include/Renderer/DirectX11/DX11Utils.h"

#include <Commctrl.h>
#include <array>


namespace JonsEngine
{
    typedef std::array<Vec4, 8> CameraFrustrum;

    static DX11RendererImpl* gDX11RendererImpl = nullptr;

    const UINT_PTR gSubClassID = 1;
    const uint32_t gTextureSamplerSlot = 0;


    Mat4 CreateDirLightVPMatrix(const CameraFrustrum& cameraFrustrum, const Vec3& lightDir)
    {
        Mat4 lightViewMatrix = glm::lookAt(Vec3(0.0f), -glm::normalize(lightDir), Vec3(0.0f, 1.0f, 0.0f));

        Vec4 transf = lightViewMatrix * cameraFrustrum[0];
        float maxZ = transf.z, minZ = transf.z;
        float maxX = transf.x, minX = transf.x;
        float maxY = transf.y, minY = transf.y;
        for (uint32_t i = 1; i < 8; i++)
        {
            transf = lightViewMatrix * cameraFrustrum[i];

            if (transf.z > maxZ) maxZ = transf.z;
            if (transf.z < minZ) minZ = transf.z;
            if (transf.x > maxX) maxX = transf.x;
            if (transf.x < minX) minX = transf.x;
            if (transf.y > maxY) maxY = transf.y;
            if (transf.y < minY) minY = transf.y;
        }

        Mat4 viewMatrix(lightViewMatrix);
        viewMatrix[3][0] = -(minX + maxX) * 0.5f;
        viewMatrix[3][1] = -(minY + maxY) * 0.5f;
        viewMatrix[3][2] = -(minZ + maxZ) * 0.5f;
        viewMatrix[0][3] = 0.0f;
        viewMatrix[1][3] = 0.0f;
        viewMatrix[2][3] = 0.0f;
        viewMatrix[3][3] = 1.0f;

        Vec3 halfExtents((maxX - minX) * 0.5, (maxY - minY) * 0.5, (maxZ - minZ) * 0.5);

        return glm::ortho(-halfExtents.x, halfExtents.x, -halfExtents.y, halfExtents.y, halfExtents.z, -halfExtents.y) * viewMatrix;
    }

    void BindTexture2D(const std::vector<DX11TexturePtr>& textures, Logger& logger, const TextureID textureID, ID3D11DeviceContext* context, uint32_t textureSlot)
    {
        auto texture = std::find_if(textures.begin(), textures.end(), [&](const DX11TexturePtr ptr) { return ptr->GetTextureID() == textureID; });
        if (texture == textures.end())
        {
            JONS_LOG_ERROR(logger, "Renderable TextureID out of range");
            throw std::runtime_error("Renderable TextureID out of range");
        }

        (*texture)->Bind(context, textureSlot);
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

                DXCALL(gDX11RendererImpl->mSwapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));

                // backbuffer rendertarget setup
                //ID3D11Texture2D* backbuffer = nullptr;
                //DXCALL(gDX11RendererImpl->mSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer));

                //DXCALL(gDX11RendererImpl->mDevice->CreateRenderTargetView(backbuffer, NULL, &gDX11RendererImpl->mBackbuffer));
                //backbuffer->Release();

                gDX11RendererImpl->SetupContext(width, height);

                return 0;
            }
            
            default:
                return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }
    }

    uint32_t ShadowQualityResolution(const EngineSettings::ShadowQuality shadowQuality)
    {
        switch (shadowQuality)
        {
        case EngineSettings::ShadowQuality::SHADOW_QUALITY_LOW:
            return 512;
        case EngineSettings::ShadowQuality::SHADOW_QUALITY_HIGH:
            return 2048;
        case EngineSettings::ShadowQuality::SHADOW_QUALITY_MEDIUM:
        default:
            return 1024;
        }
    }


    DX11RendererImpl::DX11RendererImpl(const EngineSettings& settings, Logger& logger, IMemoryAllocatorPtr memoryAllocator) : DX11Context(GetActiveWindow()), mLogger(logger), mMemoryAllocator(memoryAllocator),
        mAnisotropicFiltering(settings.mAnisotropicFiltering), mShadowQuality(settings.mShadowQuality), mBackBuffer(mDevice, mSwapchain, mSwapchainDesc.BufferDesc.Width, mSwapchainDesc.BufferDesc.Height), mGBuffer(mDevice, mSwapchainDesc.BufferDesc.Width, mSwapchainDesc.BufferDesc.Height),
        mAmbientPass(mDevice), mDirectionalLightPass(mDevice), mPointLightPass(mDevice, mBackBuffer, ShadowQualityResolution(mShadowQuality)), mTextureSampler(nullptr)
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
        
        // create blend state for shading pass
        D3D11_BLEND_DESC blendDesc;
        ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
        blendDesc.RenderTarget[0].BlendEnable = true;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        DXCALL(mDevice->CreateBlendState(&blendDesc, &mBlendState));

        SetAnisotropicFiltering(settings.mAnisotropicFiltering);
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
    
        DXCALL(mSwapchain->SetFullscreenState(false, NULL));

        mDefaultRasterizerState->Release();
        mBlendState->Release();
        mTextureSampler->Release();
    }


    MeshID DX11RendererImpl::CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData)
    {
        auto allocator = mMemoryAllocator;

        mMeshes.emplace_back(DX11MeshPtr(allocator->AllocateObject<DX11Mesh>(mDevice, vertexData, normalData, texCoords, tangents, bitangents, indexData), [=](DX11Mesh* mesh) { allocator->DeallocateObject<DX11Mesh>(mesh); }));

        return mMeshes.back()->GetMeshID();
    }

    TextureID DX11RendererImpl::CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight)
    {
        auto allocator = mMemoryAllocator;

        mTextures.emplace_back(DX11TexturePtr(allocator->AllocateObject<DX11Texture>(mDevice, mContext, textureData, textureWidth, textureHeight, textureType), [=](DX11Texture* texture) { allocator->DeallocateObject<DX11Texture>(texture); }));

        return mTextures.back()->GetTextureID();
    }

    void DX11RendererImpl::Render(const RenderQueue& renderQueue, const RenderableLighting& lighting, const DebugOptions::RenderingMode debugMode, const DebugOptions::RenderingFlags debugExtra)
    {
        GeometryStage(renderQueue);
        ShadingStage(renderQueue, lighting);

        DXCALL(mSwapchain->Present(0, 0));
    }


    EngineSettings::Anisotropic DX11RendererImpl::GetAnisotropicFiltering() const
    {
        return mAnisotropicFiltering;
    }

    void DX11RendererImpl::SetAnisotropicFiltering(const EngineSettings::Anisotropic anisotropic)
    {
        if (mTextureSampler)
        {
            mTextureSampler->Release();
            mTextureSampler = nullptr;
        }

        D3D11_SAMPLER_DESC samplerDesc;
        ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
        samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.MaxAnisotropy = anisotropic;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
        DXCALL(mDevice->CreateSamplerState(&samplerDesc, &mTextureSampler));

        mAnisotropicFiltering = anisotropic;
    }


    EngineSettings::MSAA DX11RendererImpl::GetMSAA() const
    {
        return EngineSettings::MSAA_1X;
    }

    void DX11RendererImpl::SetMSAA(const EngineSettings::MSAA msaa)
    {

    }


    float DX11RendererImpl::GetZNear() const
    {
        return Z_NEAR;
    }

    float DX11RendererImpl::GetZFar() const
    {
        return Z_FAR;
    }

    uint32_t DX11RendererImpl::GetShadowmapResolution() const
    {
        return 1024;
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

        mContext->PSSetSamplers(gTextureSamplerSlot, 1, &mTextureSampler);
    }

    void DX11RendererImpl::GeometryStage(const RenderQueue& renderQueue)
    {
        mGBuffer.BindForGeometryStage(mContext);

        auto meshIterator = mMeshes.begin();
        for (const Renderable& renderable : renderQueue)
        {
            if (renderable.mMesh == INVALID_MESH_ID)
            {
                JONS_LOG_ERROR(mLogger, "Renderable MeshID is invalid");
                throw std::runtime_error("Renderable MeshID is invalid");
            }

            if (renderable.mMesh < (*meshIterator)->GetMeshID())
                continue;

            while (renderable.mMesh >(*meshIterator)->GetMeshID())
            {
                meshIterator++;
                if (meshIterator == mMeshes.end())
                {
                    JONS_LOG_ERROR(mLogger, "Renderable MeshID out of range");
                    throw std::runtime_error("Renderable MeshID out of range");
                }
            }

            const bool hasDiffuseTexture = renderable.mDiffuseTexture != INVALID_TEXTURE_ID;
            const bool hasNormalTexture = renderable.mNormalTexture != INVALID_TEXTURE_ID;

            if (hasDiffuseTexture)
                BindTexture2D(mTextures, mLogger, renderable.mDiffuseTexture, mContext, DX11Texture::SHADER_TEXTURE_SLOT_DIFFUSE);

            if (hasNormalTexture)
                BindTexture2D(mTextures, mLogger, renderable.mNormalTexture, mContext, DX11Texture::SHADER_TEXTURE_SLOT_NORMAL);

            mGBuffer.SetConstantData(mContext, renderable.mWVPMatrix, renderable.mWorldMatrix, renderable.mTextureTilingFactor, hasDiffuseTexture, hasNormalTexture);
            (*meshIterator)->Draw(mContext);
        }
    }

    void DX11RendererImpl::ShadingStage(const RenderQueue& renderQueue, const RenderableLighting& lighting)
    {
        // geometry pass also filled gbuffer depthbuffer. We need it for further shading ops.
        ID3D11DepthStencilView* depthBuffer = mGBuffer.GetDepthStencilView();

        mBackBuffer.ClearBackbuffer(mContext);
        mBackBuffer.BindForShadingStage(mContext, depthBuffer);
        mGBuffer.BindForShadingStage(mContext);

        // ambient light
        mAmbientPass.Render(mContext, lighting.mAmbientLight);

        // additive blending for further shading
        mContext->OMSetBlendState(mBlendState, NULL, 0xffffffff);

        // do all directional lights
        mDirectionalLightPass.BindForShading(mContext);
        for (const RenderableLighting::DirectionalLight& directionalLight : lighting.mDirectionalLights)
        {
            mDirectionalLightPass.Render(mContext, directionalLight.mLightColor, directionalLight.mLightDirection);
        }

        // do all point lights
        mPointLightPass.BindForShading(mContext);
        for (const RenderableLighting::PointLight& pointLight : lighting.mPointLights)
        {
            mGBuffer.ClearStencilBuffer(mContext);
            mPointLightPass.Render(mContext, renderQueue, mMeshes, depthBuffer, pointLight);
        }

        mContext->OMSetBlendState(NULL, NULL, 0xffffffff);
    }
}