#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/RenderQueue.h"
#include "include/Core/EngineDefs.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
    class DX11VertexTransformPass;

    class DX11AABBPass
    {
    public:
        DX11AABBPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11VertexTransformPass& vertexTransformPass);
        ~DX11AABBPass();

        void Render(const RenderQueue& renderQueue, const Mat4& viewProjectionMatrix);


    private:
        ID3D11DeviceContextPtr mContext;
        ID3D11PixelShaderPtr mPixelShader;

        DX11VertexTransformPass& mVertexTransformPass;
    };
}
