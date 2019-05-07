#pragma once

#include "Renderer/DirectX11/DX11Utils.h"
#include "Core/Types.h"
#include "Core/Platform.h"

#include <vector>

namespace JonsEngine
{
    class DX11IndexBuffer
    {
    public:
        DX11IndexBuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::vector<uint16_t>& indexData);
        ~DX11IndexBuffer();

        void Bind();


    private:
        ID3D11DeviceContextPtr mContext;
        ID3D11BufferPtr mIndexBuffer;
    };
}