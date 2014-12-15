#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11LightAccumulationbuffer.h"
#include "include/Renderer/DirectX11/DX11Backbuffer.h"
#include "include/Core/Types.h"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11LightAccumulationbuffer;

    class DX11Pipeline
    {
    public:
        DX11Pipeline(ID3D11DevicePtr context, IDXGISwapChainPtr swapchain, DX11FullscreenTrianglePass& fullscreenPass);
        ~DX11Pipeline();


    private:
        DX11LightAccumulationbuffer mLightAccumulationbuffer;
        DX11Backbuffer mBackbuffer;
    };
}
