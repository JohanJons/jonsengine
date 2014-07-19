#include "include/Renderer/DirectX11/DX11RendererImpl.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/Utils/Math.h"
#include "include/Core/Utils/Win32.h"
#include "include/Renderer/DirectX11/DX11Utils.h"

#include "include/Renderer/DirectX11/Shaders/Compiled/forward_vertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/forward_pixel.h"

#include <Commctrl.h>


namespace JonsEngine
{
    static DX11RendererImpl* gDX11RendererImpl = nullptr;

    const UINT_PTR gSubClassID = 1;


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

                gDX11RendererImpl->mContext->ClearState();
                gDX11RendererImpl->mContext->OMSetRenderTargets(0, 0, 0);
                gDX11RendererImpl->mBackbuffer->Release();
                gDX11RendererImpl->mBackbuffer = nullptr;

                DXCALL(gDX11RendererImpl->mSwapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));

                // backbuffer rendertarget setup
                ID3D11Texture2D* backbuffer = nullptr;
                DXCALL(gDX11RendererImpl->mSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer));

                DXCALL(gDX11RendererImpl->mDevice->CreateRenderTargetView(backbuffer, NULL, &gDX11RendererImpl->mBackbuffer));
                backbuffer->Release();

                gDX11RendererImpl->SetupContext(width, height);

                return 0;
            }
            
            default:
                return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }
    }


    DX11RendererImpl::DX11RendererImpl(const EngineSettings& settings, Logger& logger, IMemoryAllocatorPtr memoryAllocator) : DX11Context(GetActiveWindow()), mLogger(logger), mMemoryAllocator(memoryAllocator),
        mDepthStencilBuffer(nullptr), mDepthStencilView(nullptr), mDepthStencilState(nullptr), mForwardVertexShader(nullptr), mForwardPixelShader(nullptr), mConstantBuffer(mDevice)
    {
        // backbuffer rendertarget setup
        ID3D11Texture2D* backbuffer = nullptr;
        DXCALL(mSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer));
        DXCALL(mDevice->CreateRenderTargetView(backbuffer, NULL, &mBackbuffer));
        backbuffer->Release();
        
        // setup viewport
        // query width/height from d3d
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
        DXCALL(mSwapchain->GetDesc(&swapChainDesc));

        // create shader objects
        DXCALL(mDevice->CreateVertexShader(gForwardVertexShader, sizeof(gForwardVertexShader), NULL, &mForwardVertexShader));
        DXCALL(mDevice->CreatePixelShader(gForwardPixelShader, sizeof(gForwardPixelShader), NULL, &mForwardPixelShader));

        // set CCW as front face
        D3D11_RASTERIZER_DESC rasterizerDesc;
        ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.CullMode = D3D11_CULL_BACK;
        rasterizerDesc.FrontCounterClockwise = true;
        rasterizerDesc.DepthBias = 0;
        rasterizerDesc.SlopeScaledDepthBias = 0.0f;
        rasterizerDesc.DepthBiasClamp = 0.0f;
        rasterizerDesc.DepthClipEnable = true;
        rasterizerDesc.ScissorEnable = false;
        rasterizerDesc.MultisampleEnable = false;
        rasterizerDesc.AntialiasedLineEnable = false;
        DXCALL(mDevice->CreateRasterizerState(&rasterizerDesc, &mRasterizerState));

        D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
        ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
        depthStencilBufferDesc.ArraySize = 1;
        depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilBufferDesc.CPUAccessFlags = 0;
        depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilBufferDesc.Width = swapChainDesc.BufferDesc.Width;
        depthStencilBufferDesc.Height = swapChainDesc.BufferDesc.Height;
        depthStencilBufferDesc.MipLevels = 1;
        depthStencilBufferDesc.SampleDesc.Count = 1;
        depthStencilBufferDesc.SampleDesc.Quality = 0;
        depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        DXCALL(mDevice->CreateTexture2D(&depthStencilBufferDesc, NULL, &mDepthStencilBuffer));
        DXCALL(mDevice->CreateDepthStencilView(mDepthStencilBuffer, NULL, &mDepthStencilView));

        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
        depthStencilDesc.StencilEnable = false;
        DXCALL(mDevice->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState));

        SetupContext(swapChainDesc.BufferDesc.Width, swapChainDesc.BufferDesc.Height);

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

        mDepthStencilBuffer->Release();
        mDepthStencilView->Release();
        mDepthStencilState->Release();
        mRasterizerState->Release();
        mForwardVertexShader->Release();
        mForwardPixelShader->Release();
        mBackbuffer->Release();
    }


    MeshID DX11RendererImpl::CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData)
    {
        auto allocator = mMemoryAllocator;

        mMeshes.emplace_back(DX11MeshPtr(allocator->AllocateObject<DX11Mesh>(mDevice, gForwardVertexShader, sizeof(gForwardVertexShader), vertexData, normalData, texCoords, tangents, bitangents, indexData, mLogger), [=](DX11Mesh* mesh) { allocator->DeallocateObject<DX11Mesh>(mesh); }));

        return mMeshes.back()->GetMeshID();
    }

    TextureID DX11RendererImpl::CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ColorFormat colorFormat)
    {
        auto allocator = mMemoryAllocator;

        mTextures.emplace_back(DX11TexturePtr(allocator->AllocateObject<DX11Texture>(mDevice, textureData, textureWidth, textureHeight, colorFormat, textureType, mLogger), [=](DX11Texture* texture) { allocator->DeallocateObject<DX11Texture>(texture); }));

        return mTextures.back()->GetTextureID();
    }

    void DX11RendererImpl::Render(const RenderQueue& renderQueue, const RenderableLighting& lighting, const DebugOptions::RenderingMode debugMode, const DebugOptions::RenderingFlags debugExtra)
    {
        const FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        mContext->ClearRenderTargetView(mBackbuffer, clearColor);
        mContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        Mat4 perspectiveMatrix = DX11PerspectiveMatrixFov(70.0f, 1920 / (float)1080, 0.1f, 100.0f);
        Mat4 viewMatrix = glm::lookAt(Vec3(0.0f, 4.0f, 3.5f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f));
        ConstantBufferForward buffer;
        buffer.mColor = Vec4(1.0f, 1.0f, 0.0f, 0.0f);
        buffer.mWVPMatrix = perspectiveMatrix * viewMatrix * Mat4(1.0f);

        mConstantBuffer.SetData(buffer, mContext);

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

            while (renderable.mMesh > (*meshIterator)->GetMeshID())
            {
                meshIterator++;
                if (meshIterator == mMeshes.end())
                {
                    JONS_LOG_ERROR(mLogger, "Renderable MeshID out of range");
                    throw std::runtime_error("Renderable MeshID out of range");
                }
            }

            (*meshIterator)->Draw(mContext);
        }

        DXCALL(mSwapchain->Present(0, 0));
    }


    EngineSettings::Anisotropic DX11RendererImpl::GetAnisotropicFiltering() const
    {
        return EngineSettings::ANISOTROPIC_1X;
    }

    void DX11RendererImpl::SetAnisotropicFiltering(const EngineSettings::Anisotropic anisotropic)
    {
        
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
        return 0.0f;
    }

    float DX11RendererImpl::GetZFar() const
    {
        return 1.0f;
    }

    uint32_t DX11RendererImpl::GetShadowmapResolution() const
    {
        return 1024;
    }


    void DX11RendererImpl::SetupContext(const uint32_t viewportWidth, const uint32_t viewportHeight)
    {

        mContext->OMSetRenderTargets(1, &mBackbuffer, mDepthStencilView);
        mContext->OMSetDepthStencilState(mDepthStencilState, 1);
        mContext->RSSetState(mRasterizerState);

        D3D11_VIEWPORT viewport;
        ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = static_cast<float>(viewportWidth);
        viewport.Height = static_cast<float>(viewportHeight);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        mContext->RSSetViewports(1, &viewport);

        mContext->VSSetShader(mForwardVertexShader, NULL, NULL);
        mContext->PSSetShader(mForwardPixelShader, NULL, NULL);
    }
}