#pragma once

#include "include/Renderer/RenderQueue.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Containers/RangedIterator.hpp"
#include "include/Core/Platform/Directx11.h"

#include <vector>
#include <array>

namespace JonsEngine
{
    class DX11VertexTransformPass
    {
    public:
        DX11VertexTransformPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, IDMap<DX11Mesh>& meshMap);
        ~DX11VertexTransformPass();

        void BindForTransformPass(const D3D_PRIMITIVE_TOPOLOGY primitiveTopology);
        void RenderMesh(DX11Mesh& mesh, const Mat4& wvpMatrix);
        void RenderMeshes(const RenderableMeshes& meshes, const Mat4& viewProjectionMatrix);
        void RenderMeshes(ConstRangedIterator<RenderableMeshes>& meshIterator, const Mat4& viewProjectionMatrix);
        void RenderAABBs(const RenderableModels& models, const Mat4& viewProjectionMatrix);


    private:
        struct TransformCBuffer
        {
            Mat4 mWVPMatrix;


            TransformCBuffer(const Mat4& wvpMatrix) : mWVPMatrix(wvpMatrix)
            {
            }
        };

        template <typename T>
        void RenderMeshesAux(const T& meshes, const Mat4& viewProjectionMatrix);


        ID3D11DeviceContextPtr mContext;
        ID3D11VertexShaderPtr mVertexShader;
        ID3D11InputLayoutPtr mInputLayout;

        DX11ConstantBuffer<TransformCBuffer> mTransformCBuffer;

        IDMap<DX11Mesh>& mMeshMap;
    };
}
