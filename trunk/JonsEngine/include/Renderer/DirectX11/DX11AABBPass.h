#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/RenderCommands.h"
#include "include/Core/EngineDefs.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
    class DX11VertexTransformPass;

    class DX11AABBPass
    {
    public:
        DX11AABBPass(ID3D11DevicePtr device, DX11VertexTransformPass& vertexTransformPass);
        ~DX11AABBPass();

        void Render(ID3D11DeviceContextPtr context, const RenderQueue& renderQueue, const std::vector<DX11MeshPtr>& meshes, const Mat4& viewProjectionMatrix);


    private:
        DX11VertexTransformPass& mVertexTransformPass;
        ID3D11PixelShaderPtr mPixelShader;
    };
}
