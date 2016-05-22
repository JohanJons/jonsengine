#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/RenderQueue/RenderQueue.h"
#include "include/RenderQueue/RenderableCamera.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
    struct RenderQueue;
    class DX11VertexTransformPass;

	// TODO: refactor into something else?

    class DX11AABBPass
    {
    public:
        DX11AABBPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11VertexTransformPass& vertexTransformPass);
        ~DX11AABBPass();

        void Render(const RenderQueue::RenderData& renderData, const RenderableCamera& camera);


    private:
        ID3D11DeviceContextPtr mContext;
        ID3D11PixelShaderPtr mPixelShader;

        DX11VertexTransformPass& mVertexTransformPass;
    };
}
