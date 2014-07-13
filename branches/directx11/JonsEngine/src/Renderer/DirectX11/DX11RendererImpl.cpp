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

	struct VERTEX
    {
        FLOAT X, Y, Z;      // position
    };


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


    DX11RendererImpl::DX11RendererImpl(const EngineSettings& settings, Logger& logger) : mLogger(logger), mWindowHandle(GetActiveWindow()), mSwapchain(nullptr), mBackbuffer(nullptr), mDevice(nullptr), mContext(nullptr),
        mForwardVertexShader(nullptr), mForwardPixelShader(nullptr), mVertexBuffer(nullptr), mIndexBuffer(nullptr), mInputLayout(nullptr)
    {
        // create swapchain, device and devicecontext
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
        swapChainDesc.BufferCount = 2;
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = mWindowHandle;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.Windowed = true;

        const D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
        const UINT numFeatureLevels = 1;

        DXCALL(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, numFeatureLevels, D3D11_SDK_VERSION, &swapChainDesc, &mSwapchain, &mDevice, NULL, &mContext));

        // backbuffer rendertarget setup
        ID3D11Texture2D* backbuffer = nullptr;
        DXCALL(mSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer));

        DXCALL(mDevice->CreateRenderTargetView(backbuffer, NULL, &mBackbuffer));
        backbuffer->Release();
        
        // setup viewport
        // query width/height from d3d
        ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
        DXCALL(mSwapchain->GetDesc(&swapChainDesc));

        // create shader objects
        DXCALL(mDevice->CreateVertexShader(gForwardVertexShader, sizeof(gForwardVertexShader), NULL, &mForwardVertexShader));
        DXCALL(mDevice->CreatePixelShader(gForwardPixelShader, sizeof(gForwardPixelShader), NULL, &mForwardPixelShader));

        // vertex buffer
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

		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
		initData.pSysMem = OurVertices;

		DXCALL(mDevice->CreateBuffer(&bufferDescription, &initData, &mVertexBuffer));

        // index buffer
        uint16_t indices[] = {0, 1, 2};

        ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
        bufferDescription.Usage = D3D11_USAGE_DEFAULT;
        bufferDescription.ByteWidth = sizeof(indices);
        bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;

        ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
        initData.pSysMem = indices;

        DXCALL(mDevice->CreateBuffer(&bufferDescription, &initData, &mIndexBuffer));

		D3D11_INPUT_ELEMENT_DESC inputDescription;
		ZeroMemory(&inputDescription, sizeof(D3D11_INPUT_ELEMENT_DESC));
		inputDescription.SemanticName = "POSITION";
		inputDescription.SemanticIndex = 0;
		inputDescription.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputDescription.InputSlot = 0;
		inputDescription.AlignedByteOffset = 0;
		inputDescription.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputDescription.InstanceDataStepRate = 0;

        DXCALL(mDevice->CreateInputLayout(&inputDescription, 1, gForwardVertexShader, sizeof(gForwardVertexShader), &mInputLayout));
        
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

		mInputLayout->Release();
        mIndexBuffer->Release();
		mVertexBuffer->Release();
        mForwardVertexShader->Release();
        mForwardPixelShader->Release();
        mSwapchain->Release();
        mBackbuffer->Release();
        mDevice->Release();
        mContext->Release();
    }


    MeshID DX11RendererImpl::CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData)
    {
        return INVALID_MESH_ID;
    }

    TextureID DX11RendererImpl::CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ColorFormat colorFormat)
    {
        return INVALID_TEXTURE_ID;
    }


    void DX11RendererImpl::Render(const RenderQueue& renderQueue, const RenderableLighting& lighting, const DebugOptions::RenderingMode debugMode, const DebugOptions::RenderingFlags debugExtra)
    {
        const FLOAT clearColor[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
        mContext->ClearRenderTargetView(mBackbuffer, clearColor);

		uint32_t vertexSize = sizeof(VERTEX);
		uint32_t offset = 0;
		mContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &vertexSize, &offset);
        mContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        mContext->DrawIndexed(3, 0, 0);

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
        mContext->OMSetRenderTargets(1, &mBackbuffer, NULL);

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

        mContext->IASetInputLayout(mInputLayout);
    }
}