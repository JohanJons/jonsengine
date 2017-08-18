#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
    struct AABBRenderData;
    class DX11VertexTransformPass;

	// TODO: refactor into something else?

    class DX11AABBPass
    {
    public:
        DX11AABBPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11VertexTransformPass& vertexTransformPass);
        ~DX11AABBPass();

        void Render(const AABBRenderData& renderData);


    private:
        ID3D11DeviceContextPtr mContext;
        ID3D11PixelShaderPtr mPixelShader;

		DX11Mesh mAABBMesh;

        DX11VertexTransformPass& mVertexTransformPass;
    };
}
