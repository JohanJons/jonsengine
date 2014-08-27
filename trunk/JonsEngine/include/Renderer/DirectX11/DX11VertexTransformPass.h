#pragma once

#include "include/Renderer/RenderCommands.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Types.h"

#include <d3d11.h>
#include <vector>
#include <array>

namespace JonsEngine
{
    class DX11VertexTransformPass
    {
    public:
        DX11VertexTransformPass(ID3D11DevicePtr device);
        ~DX11VertexTransformPass();

        void BindForTransformPass(ID3D11DeviceContextPtr context);
        void RenderMesh(ID3D11DeviceContextPtr context, DX11Mesh& mesh, const Mat4& wvpMatrix);
        void RenderMeshes(ID3D11DeviceContextPtr context, const RenderQueue& renderQueue, std::vector<DX11MeshPtr>& meshes, const Mat4& viewProjectionMatrix);


    private:
        struct TransformCBuffer
        {
            Mat4 mWVPMatrix;


            TransformCBuffer(const Mat4& wvpMatrix) : mWVPMatrix(wvpMatrix)
            {
            }
        };

        ID3D11VertexShaderPtr mVertexShader;
        ID3D11InputLayoutPtr mInputLayout;
        DX11ConstantBuffer<TransformCBuffer> mTransformCBuffer;
    };
}
