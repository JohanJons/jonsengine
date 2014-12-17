#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11GBuffer.h"
#include "include/Renderer/DirectX11/DX11LightAccumulationbuffer.h"
#include "include/Renderer/DirectX11/DX11Backbuffer.h"
#include "include/Core/Types.h"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11Pipeline
    {
    public:
        DX11Pipeline(ID3D11DevicePtr device, D3D11_TEXTURE2D_DESC backbufferTextureDesc);
        ~DX11Pipeline();


    private:
        DX11Backbuffer mBackbuffer;
        DX11LightAccumulationbuffer mLightAccbuffer;
        DX11GBuffer mGBuffer;
    };
}
