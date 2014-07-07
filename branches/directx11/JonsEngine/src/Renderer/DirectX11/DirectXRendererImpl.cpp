#include "include/Renderer/DirectX11/DirectXRendererImpl.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/Utils/Math.h"

#include <Commctrl.h>


namespace JonsEngine
{
    static DirectXRendererImpl* gDirectXRendererImpl = nullptr;

    const UINT_PTR gSubClassID = 1;
    
    
    LRESULT CALLBACK DirectXRendererImpl::OwnerDrawButtonProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
    {
        switch (uMsg)
        {
            case WM_SIZE:
            {
                WORD width = LOWORD(lParam);
                WORD height = HIWORD(lParam);

                return 0;
            }
            
            default:
                return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }
    }


    DirectXRendererImpl::DirectXRendererImpl(const EngineSettings& settings, Logger& logger) : mLogger(logger), mWindowHandle(GetActiveWindow()), mSwapchain(nullptr), mBackbuffer(nullptr), mDevice(nullptr), mDeviceContext(nullptr)
    {
        // create swapchain, device and devicecontext
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
        swapChainDesc.BufferCount = 2;
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = mWindowHandle;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.Windowed = true;

        const D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
        const UINT numFeatureLevels = 1;

        HRESULT result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, numFeatureLevels, D3D11_SDK_VERSION, &swapChainDesc, &mSwapchain, &mDevice, NULL, &mDeviceContext);
        if (result != S_OK)
        {
            JONS_LOG_ERROR(mLogger, "DirectXRenderer::DirectXRenderer(): D3D11CreateDeviceAndSwapChain failed: code " + result);
            throw std::runtime_error("DirectXRenderer::DirectXRenderer(): D3D11CreateDeviceAndSwapChain failed: code " + result);
        }

        // backbuffer rendertarget setup
        ID3D11Texture2D* backbuffer;
        mSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);

        mDevice->CreateRenderTargetView(backbuffer, nullptr, &mBackbuffer);
        backbuffer->Release();

        mDeviceContext->OMSetRenderTargets(1, &mBackbuffer, nullptr);

        // setup viewport
        D3D11_VIEWPORT viewport;
        ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = static_cast<FLOAT>(settings.mWindowWidth);
        viewport.Height = static_cast<FLOAT>(settings.mWindowHeight);

        mDeviceContext->RSSetViewports(1, &viewport);

        // register as window subclass to listen for WM_SIZE events. etc
        if (!SetWindowSubclass(mWindowHandle, OwnerDrawButtonProc, gSubClassID, 0))
        {
            JONS_LOG_ERROR(mLogger, "DirectXRenderer::DirectXRenderer(): SetWindowSubclass() failed");
            throw std::runtime_error("DirectXRenderer::DirectXRenderer(): SetWindowSubclass() failed");
        }
        
        gDirectXRendererImpl = this;
    }

    DirectXRendererImpl::~DirectXRendererImpl()
    {
        RemoveWindowSubclass(mWindowHandle, OwnerDrawButtonProc, gSubClassID);
    
        mSwapchain->SetFullscreenState(false, nullptr);    // needed?

        mSwapchain->Release();
        mBackbuffer->Release();
        mDevice->Release();
        mDeviceContext->Release();
    }


    MeshID DirectXRendererImpl::CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData)
    {
        return INVALID_MESH_ID;
    }

    TextureID DirectXRendererImpl::CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ColorFormat colorFormat)
    {
        return INVALID_TEXTURE_ID;
    }


    void DirectXRendererImpl::Render(const RenderQueue& renderQueue, const RenderableLighting& lighting, const DebugOptions::RenderingMode debugMode, const DebugOptions::RenderingFlags debugExtra)
    {
        const FLOAT clearColor[4] = { 1.0f, 1.0f, 0.0f, 1.0f };

        mDeviceContext->ClearRenderTargetView(mBackbuffer, clearColor);

        mSwapchain->Present(0, 0);
    }


    EngineSettings::Anisotropic DirectXRendererImpl::GetAnisotropicFiltering() const
    {
        return EngineSettings::ANISOTROPIC_1X;
    }

    void DirectXRendererImpl::SetAnisotropicFiltering(const EngineSettings::Anisotropic anisotropic)
    {
        const FLOAT clearColor[4] = {0.2f, 0.5f, 0.0f, 1.0f};

        mDeviceContext->ClearRenderTargetView(mBackbuffer, clearColor);

        mSwapchain->Present(0, 0);
    }


    EngineSettings::MSAA DirectXRendererImpl::GetMSAA() const
    {
        return EngineSettings::MSAA_1X;
    }

    void DirectXRendererImpl::SetMSAA(const EngineSettings::MSAA msaa)
    {

    }


    float DirectXRendererImpl::GetZNear() const
    {
        return 0.0f;
    }

    float DirectXRendererImpl::GetZFar() const
    {
        return 1.0f;
    }

    uint32_t DirectXRendererImpl::GetShadowmapResolution() const
    {
        return 1024;
    }
}