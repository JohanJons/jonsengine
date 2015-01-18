#pragma once

#include "include/Renderer/RenderQueue.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.h"

#include <d3d11.h>
#include <vector>
#include <array>

namespace JonsEngine
{
    class DX11VertexTransformPass
    {
    public:
        DX11VertexTransformPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const IDMap<DX11Mesh>& meshMap);
        ~DX11VertexTransformPass();

        void BindForTransformPass(const D3D_PRIMITIVE_TOPOLOGY primitiveTopology);
        void RenderMesh(DX11Mesh& mesh, const Mat4& wvpMatrix);
        void RenderMeshes(const RenderQueue& renderQueue, const Mat4& viewProjectionMatrix);
        void RenderAABBs(const RenderQueue& renderQueue, const Mat4& viewProjectionMatrix);


    private:
        struct TransformCBuffer
        {
            Mat4 mWVPMatrix;


            TransformCBuffer(const Mat4& wvpMatrix) : mWVPMatrix(wvpMatrix)
            {
            }
        };

        ID3D11DeviceContextPtr mContext;
        ID3D11VertexShaderPtr mVertexShader;
        ID3D11InputLayoutPtr mInputLayout;

        DX11ConstantBuffer<TransformCBuffer> mTransformCBuffer;

        const IDMap<DX11Mesh>& mMeshMap;
    };
}
