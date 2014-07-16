#include "include/Renderer/DirectX11/DX11Context.h"

#include "include/Renderer/DirectX11/DX11Utils.h"

namespace JonsEngine
{
    DX11Context::DX11Context(const HWND windowHandle) : mWindowHandle(windowHandle), mSwapchain(nullptr), mBackbuffer(nullptr), mDevice(nullptr), mContext(nullptr)
    {
        const D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
        const UINT numFeatureLevels = 1;

        // create swapchain, device and devicecontext
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
        swapChainDesc.BufferCount = 2;
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = mWindowHandle;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.Windowed = true;

        DXCALL(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, numFeatureLevels, D3D11_SDK_VERSION, &swapChainDesc, &mSwapchain, &mDevice, NULL, &mContext));
    }

    DX11Context::~DX11Context()
    {
        mSwapchain->Release();
        mDevice->Release();
        mContext->Release();
    }
}