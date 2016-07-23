#include "include/Renderer/DirectX11/DX11VertexTransformPass.h"

#include "include/Renderer/DirectX11/Shaders/Compiled/TransformVertex.h"

namespace JonsEngine
{
    DX11VertexTransformPass::DX11VertexTransformPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, IDMap<DX11Mesh>& meshMap) :
        mContext(context), mTransformCBuffer(device, context, mTransformCBuffer.CONSTANT_BUFFER_SLOT_VERTEX), mMeshMap(meshMap)
    {
        D3D11_INPUT_ELEMENT_DESC inputDescription;
        ZeroMemory(&inputDescription, sizeof(D3D11_INPUT_ELEMENT_DESC));
        inputDescription.SemanticName = "POSITION";
        inputDescription.SemanticIndex = 0;
        inputDescription.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputDescription.InputSlot = 0;
        inputDescription.AlignedByteOffset = 0;
        inputDescription.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription.InstanceDataStepRate = 0;
        DXCALL(device->CreateInputLayout(&inputDescription, 1, gTransformVertexShader, sizeof(gTransformVertexShader), &mInputLayout));

        DXCALL(device->CreateVertexShader(gTransformVertexShader, sizeof(gTransformVertexShader), nullptr, &mVertexShader));
    }

    DX11VertexTransformPass::~DX11VertexTransformPass()
    {
    }


    void DX11VertexTransformPass::BindForTransformPass(const D3D_PRIMITIVE_TOPOLOGY primitiveTopology)
    {
        mContext->IASetPrimitiveTopology(primitiveTopology);
        mContext->IASetInputLayout(mInputLayout);

        mContext->VSSetShader(mVertexShader, nullptr, 0);
    }

    void DX11VertexTransformPass::RenderMesh(DX11Mesh& mesh, const Mat4& wvpMatrix)
    {
        mTransformCBuffer.SetData(TransformCBuffer(wvpMatrix));
        mesh.DrawPositions();
    }

    template <typename T>
    void DX11VertexTransformPass::RenderMeshesAux(const T& meshContainer, const Mat4& viewProjectionMatrix)
    {
        for (const RenderableMesh& mesh : meshContainer)
        {
            mTransformCBuffer.SetData(TransformCBuffer(viewProjectionMatrix * mesh.mWorldTransform));
            mMeshMap.GetItem(mesh.mMeshID).DrawPositions();
        }
    }


	void DX11VertexTransformPass::RenderMeshes(const  RenderQueue::RenderData& renderData, const RenderableMesh::Index beginIndex, const RenderableMesh::Index endIndex, const Mat4& viewProjectionMatrix)
	{
		for (uint32_t meshIndex = beginIndex; meshIndex < endIndex; ++meshIndex)
		{
			const RenderableMesh& mesh = renderData.mStaticMeshes.at(meshIndex);
			mTransformCBuffer.SetData(TransformCBuffer(viewProjectionMatrix * mesh.mWorldTransform));
			mMeshMap.GetItem(mesh.mMeshID).DrawPositions();
		}
	}

	void DX11VertexTransformPass::RenderAABBs(const  RenderQueue::RenderData& renderData, const RenderableMesh::Index beginIndex, const RenderableMesh::Index endIndex, const Mat4& viewProjectionMatrix)
	{
		for (uint32_t meshIndex = beginIndex; meshIndex < endIndex; ++meshIndex)
		{
			const RenderableMesh& mesh = renderData.mStaticMeshes.at(meshIndex);
			mTransformCBuffer.SetData(TransformCBuffer(viewProjectionMatrix * mesh.mWorldTransform));
			mMeshMap.GetItem(mesh.mMeshID).DrawAABB();
		}
	}
	/*
    void DX11VertexTransformPass::RenderMeshes(const RenderableMeshes& meshes, const Mat4& viewProjectionMatrix)
    {
        RenderMeshesAux(meshes, viewProjectionMatrix);
    }

    void DX11VertexTransformPass::RenderMeshes(ConstRangedIterator<RenderableMeshes>& meshIterator, const Mat4& viewProjectionMatrix)
    {
        RenderMeshesAux(meshIterator, viewProjectionMatrix);
    }

    void DX11VertexTransformPass::RenderAABBs(const RenderableModels& models, const Mat4& viewProjectionMatrix)
    {
        for (const RenderableModel& model : models)
        {
            mTransformCBuffer.SetData(TransformCBuffer(viewProjectionMatrix * model.mMesh.mWorldTransform));
            mMeshMap.GetItem(model.mMesh.mMeshID).DrawAABB();
        }
    }*/
}