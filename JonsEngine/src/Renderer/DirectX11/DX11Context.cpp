#include "include/Renderer/DirectX11/DX11Context.h"

#include "include/Renderer/DirectX11/DX11Utils.h"

namespace JonsEngine
{
    DX11Context::DX11Context(const HWND windowHandle) : mWindowHandle(windowHandle), mSwapchain(nullptr), mDevice(nullptr), mContext(nullptr)
    {
        const D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
        const UINT numFeatureLevels = 1;
        uint32_t deviceFlags = 0;
        #if _DEBUG
            deviceFlags = D3D11_CREATE_DEVICE_DEBUG;
        #endif

        // create swapchain, device and devicecontext
        ZeroMemory(&mSwapchainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
        mSwapchainDesc.BufferCount = 2;
        mSwapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        mSwapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        mSwapchainDesc.OutputWindow = mWindowHandle;
        mSwapchainDesc.SampleDesc.Count = 1;
        mSwapchainDesc.Windowed = true;
        DXCALL(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, deviceFlags, &featureLevel, numFeatureLevels, D3D11_SDK_VERSION, &mSwapchainDesc, &mSwapchain, &mDevice, NULL, &mContext));

        ZeroMemory(&mSwapchainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
        DXCALL(mSwapchain->GetDesc(&mSwapchainDesc));
    }

    DX11Context::~DX11Context()
    {
    }


    D3D11_TEXTURE2D_DESC DX11Context::GetBackbufferTextureDesc() const
    {
        D3D11_TEXTURE2D_DESC desc;

        ID3D11Texture2DPtr backbuffer = nullptr;
        DXCALL(mSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer));
        backbuffer->GetDesc(&desc);

        return desc;
    }
}