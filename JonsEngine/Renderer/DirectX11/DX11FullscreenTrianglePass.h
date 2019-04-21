#pragma once

#include "Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "Renderer/DirectX11/DX11Utils.h"
#include "Core/Types.h"
#include "Core/Platform.h"

namespace JonsEngine
{
    class DX11FullscreenTrianglePass
    {
    public:
        DX11FullscreenTrianglePass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context);
        ~DX11FullscreenTrianglePass();

        void Render();
		void RenderWithTexcoords();


    private:
		void InternalRender();

        ID3D11DeviceContextPtr mContext;
        ID3D11VertexShaderPtr mFSVertexShader;
		ID3D11VertexShaderPtr mFSTexcoordVertexShader;
    };
}
