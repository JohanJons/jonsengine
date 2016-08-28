#pragma once

#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/RenderQueue/RenderQueue.h"
#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Platform/Directx11.h"

#include <vector>
#include <array>

namespace JonsEngine
{
	struct AABBRenderData;
	struct RenderableCollection;

    class DX11VertexTransformPass
    {
    public:
        DX11VertexTransformPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, IDMap<DX11Mesh>& meshMap);
        ~DX11VertexTransformPass();

        void RenderStaticMesh(DX11Mesh& mesh, const Mat4& wvpMatrix);
		void RenderMeshes(const RenderQueue::RenderData& renderData, const RenderableCollection& renderables, const Mat4& viewProjectionMatrix);
		void RenderAABBs(const AABBRenderData& aabbRenderData);


    private:
        struct TransformCBuffer
        {
            Mat4 mWVPMatrix;


            TransformCBuffer(const Mat4& wvpMatrix) : mWVPMatrix(wvpMatrix)
            {
            }
        };

		void BindForStaticRendering();
		void BindForAnimatedRendering();
        void RenderMeshesAux(const RenderableMesh::ContainerType& meshContainer, const RenderableMesh::Index beginIndex, const RenderableMesh::Index endIndex, const Mat4& viewProjectionMatrix);
		void RenderAABBsAux(const RenderableMesh::ContainerType& meshContainer, const RenderableMesh::Index beginIndex, const RenderableMesh::Index endIndex, const Mat4& viewProjectionMatrix);


        ID3D11DeviceContextPtr mContext;
        ID3D11VertexShaderPtr mStaticShader;
		ID3D11VertexShaderPtr mAnimatedShader;
        ID3D11InputLayoutPtr mLayoutStatic;
		ID3D11InputLayoutPtr mLayoutAnimated;

        DX11ConstantBuffer<TransformCBuffer> mTransformCBuffer;

        IDMap<DX11Mesh>& mMeshMap;
    };
}
