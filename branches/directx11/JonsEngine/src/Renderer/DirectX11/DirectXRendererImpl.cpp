#include "include/Renderer/DirectX11/DirectXRendererImpl.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/Utils/Math.h"
#include "include/Core/Utils/Win32.h"

#include "include/Renderer/DirectX11/Shaders/Compiled/forward_vertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/forward_pixel.h"

#include <Commctrl.h>


namespace JonsEngine
{
    static DirectXRendererImpl* gDirectXRendererImpl = nullptr;

    const UINT_PTR gSubClassID = 1;

	struct VERTEX
    {
        FLOAT X, Y, Z;      // position
    };


    LRESULT CALLBACK DirectXRendererImpl::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
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
                    gDirectXRendererImpl->mSwapchain->SetFullscreenState(wParam, NULL);
                    gDirectXRendererImpl->mSwapchain->ResizeTarget(&displayDesc);
                }
                else
                {
                    gDirectXRendererImpl->mSwapchain->ResizeTarget(&displayDesc);
                    gDirectXRendererImpl->mSwapchain->SetFullscreenState(wParam, NULL);
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

                gDirectXRendererImpl->mSwapchain->ResizeTarget(&displayDesc);

				return 0;
			}

            case WM_SIZE:
            {
                const uint16_t width = LOWORD(lParam);
                const uint16_t height = HIWORD(lParam);

                gDirectXRendererImpl->mContext->ClearState();
                gDirectXRendererImpl->mContext->OMSetRenderTargets(0, 0, 0);
                gDirectXRendererImpl->mBackbuffer->Release();
                gDirectXRendererImpl->mBackbuffer = nullptr;

                HRESULT result = gDirectXRendererImpl->mSwapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

                // backbuffer rendertarget setup
                ID3D11Texture2D* backbuffer = nullptr;
                gDirectXRendererImpl->mSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);

                gDirectXRendererImpl->mDevice->CreateRenderTargetView(backbuffer, NULL, &gDirectXRendererImpl->mBackbuffer);
                backbuffer->Release();

                gDirectXRendererImpl->mContext->OMSetRenderTargets(1, &gDirectXRendererImpl->mBackbuffer, NULL);

                D3D11_VIEWPORT vp;
                vp.Width = width;
                vp.Height = height;
                vp.MinDepth = 0.0f;
                vp.MaxDepth = 1.0f;
                vp.TopLeftX = 0;
                vp.TopLeftY = 0;
                gDirectXRendererImpl->mContext->RSSetViewports(1, &vp);


                return 0;
            }
            
            default:
                return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }
    }


    DirectXRendererImpl::DirectXRendererImpl(const EngineSettings& settings, Logger& logger) : mLogger(logger), mWindowHandle(GetActiveWindow()), mSwapchain(nullptr), mBackbuffer(nullptr), mDevice(nullptr), mContext(nullptr),
        mForwardVertexShader(nullptr), mForwardPixelShader(nullptr), mVertexBuffer(nullptr), mInputLayout(nullptr)
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

        HRESULT result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, numFeatureLevels, D3D11_SDK_VERSION, &swapChainDesc, &mSwapchain, &mDevice, NULL, &mContext);
        if (result != S_OK)
        {
            JONS_LOG_ERROR(mLogger, "DirectXRenderer::DirectXRenderer(): D3D11CreateDeviceAndSwapChain failed: code " + result);
            throw std::runtime_error("DirectXRenderer::DirectXRenderer(): D3D11CreateDeviceAndSwapChain failed: code " + result);
        }

        // backbuffer rendertarget setup
        ID3D11Texture2D* backbuffer = nullptr;
        mSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);

        mDevice->CreateRenderTargetView(backbuffer, NULL, &mBackbuffer);
        backbuffer->Release();

        mContext->OMSetRenderTargets(1, &mBackbuffer, NULL);
        
        // setup viewport
        // query width/height from d3d
        ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
        mSwapchain->GetDesc(&swapChainDesc);

        D3D11_VIEWPORT viewport;
        ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = static_cast<float>(swapChainDesc.BufferDesc.Width);
        viewport.Height = static_cast<float>(swapChainDesc.BufferDesc.Height);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        mContext->RSSetViewports(1, &viewport);

        // create shader objects
        mDevice->CreateVertexShader(gForwardVertexShader, sizeof(gForwardVertexShader), NULL, &mForwardVertexShader);
        mDevice->CreatePixelShader(gForwardPixelShader, sizeof(gForwardPixelShader), NULL, &mForwardPixelShader);

        mContext->VSSetShader(mForwardVertexShader, NULL, NULL);
        mContext->PSSetShader(mForwardPixelShader, NULL, NULL);

		// fill vertex buffer
		VERTEX OurVertices[] =
        {
            { 0.0f, 0.5f, 0.0f },
            { 0.45f, -0.5, 0.0f},
            { -0.45f, -0.5f, 0.0f }
        };

		D3D11_BUFFER_DESC bufferDescription;
		ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
		bufferDescription.Usage = D3D11_USAGE_DEFAULT;
		bufferDescription.ByteWidth = sizeof(OurVertices);
		bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDescription.CPUAccessFlags = 0;
		bufferDescription.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
		initData.pSysMem = OurVertices;

		mDevice->CreateBuffer(&bufferDescription, &initData, &mVertexBuffer);

		D3D11_INPUT_ELEMENT_DESC inputDescription;
		ZeroMemory(&inputDescription, sizeof(D3D11_INPUT_ELEMENT_DESC));
		inputDescription.SemanticName = "POSITION";
		inputDescription.SemanticIndex = 0;
		inputDescription.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputDescription.InputSlot = 0;
		inputDescription.AlignedByteOffset = 0;
		inputDescription.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputDescription.InstanceDataStepRate = 0;

		mDevice->CreateInputLayout(&inputDescription, 1, gForwardVertexShader, sizeof(gForwardVertexShader), &mInputLayout);

        // register as window subclass to listen for WM_SIZE events. etc
        if (!SetWindowSubclass(mWindowHandle, WndProc, gSubClassID, 0))
        {
            JONS_LOG_ERROR(mLogger, "DirectXRenderer::DirectXRenderer(): SetWindowSubclass() failed");
            throw std::runtime_error("DirectXRenderer::DirectXRenderer(): SetWindowSubclass() failed");
        }

        gDirectXRendererImpl = this;
    }

    DirectXRendererImpl::~DirectXRendererImpl()
    {
        RemoveWindowSubclass(mWindowHandle, WndProc, gSubClassID);
    
        mSwapchain->SetFullscreenState(false, NULL);

		mInputLayout->Release();
		mVertexBuffer->Release();
        mForwardVertexShader->Release();
        mForwardPixelShader->Release();
        mSwapchain->Release();
        mBackbuffer->Release();
        mDevice->Release();
        mContext->Release();
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
        mContext->ClearRenderTargetView(mBackbuffer, clearColor);

		uint32_t vertexSize = sizeof(VERTEX);
		uint32_t offset = 0;
		mContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &vertexSize, &offset);
		mContext->IASetInputLayout(mInputLayout);
		mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mContext->Draw(3, 0);

        mSwapchain->Present(0, 0);
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