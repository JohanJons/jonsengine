#pragma once

#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/RenderQueue/RenderableMesh.h"
#include "include/RenderQueue/RenderQueue.h"
#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Platform/Directx11.h"

#include <vector>
#include <array>

namespace JonsEngine
{
	struct RenderData;
	struct RenderableCollection;

    class DX11VertexTransformPass
    {
    public:
        DX11VertexTransformPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, IDMap<DX11Mesh>& meshMap);
        ~DX11VertexTransformPass();

        void BindForTransformPass(const D3D_PRIMITIVE_TOPOLOGY primitiveTopology);
        void RenderMesh(DX11Mesh& mesh, const Mat4& wvpMatrix, const bool isAnimating);
		void RenderMeshes(const RenderQueue::RenderData& renderData, const RenderableCollection& renderables, const Mat4& viewProjectionMatrix);
		void RenderAABBs(const RenderQueue::RenderData& renderData, const RenderableCollection& renderables, const Mat4& viewProjectionMatrix);


    private:
        struct TransformCBuffer
        {
            Mat4 mWVPMatrix;
			uint32_t mIsAnimating;


            TransformCBuffer(const Mat4& wvpMatrix, const bool isAnimating) : mWVPMatrix(wvpMatrix), mIsAnimating(static_cast<uint32_t>(isAnimating))
            {
            }
        };

        void RenderMeshesAux(const RenderableMesh::ContainerType& meshContainer, const RenderableMesh::Index beginIndex, const RenderableMesh::Index endIndex, const Mat4& viewProjectionMatrix, const bool isAnimating);
		void RenderAABBsAux(const RenderableMesh::ContainerType& meshContainer, const RenderableMesh::Index beginIndex, const RenderableMesh::Index endIndex, const Mat4& viewProjectionMatrix);


        ID3D11DeviceContextPtr mContext;
        ID3D11VertexShaderPtr mVertexShader;
        ID3D11InputLayoutPtr mInputLayout;

        DX11ConstantBuffer<TransformCBuffer> mTransformCBuffer;

        IDMap<DX11Mesh>& mMeshMap;
    };
}
