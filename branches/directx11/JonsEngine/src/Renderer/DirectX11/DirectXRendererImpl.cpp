#include "include/Renderer/DirectX11/DirectXRendererImpl.h"

#include "include/Core/Logging/Logger.h"


namespace JonsEngine
{
    DirectXRendererImpl::DirectXRendererImpl(const EngineSettings& settings, Logger& logger) : mLogger(logger), mSwapchain(nullptr), mBackbuffer(nullptr), mDevice(nullptr), mDeviceContext(nullptr)
    {
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
        swapChainDesc.BufferCount = 2;        // front + back buffer
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = GetActiveWindow();
        swapChainDesc.SampleDesc.Count = 1;           // deferred MSAA instead
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
    }

    DirectXRendererImpl::~DirectXRendererImpl()
    {
        mSwapchain->Release();
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

    }


    EngineSettings::Anisotropic DirectXRendererImpl::GetAnisotropicFiltering() const
    {
        return EngineSettings::ANISOTROPIC_1X;
    }

    void DirectXRendererImpl::SetAnisotropicFiltering(const EngineSettings::Anisotropic anisotropic)
    {

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