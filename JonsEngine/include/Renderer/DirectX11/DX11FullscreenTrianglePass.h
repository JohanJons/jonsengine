#pragma once

#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Types.h"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11FullscreenTrianglePass
    {
    public:
        DX11FullscreenTrianglePass(ID3D11DevicePtr device);
        ~DX11FullscreenTrianglePass();

        void Render(ID3D11DeviceContextPtr context);


    private:
        ID3D11VertexShaderPtr mVertexShader;
    };
}
