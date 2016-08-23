#include "include/Renderer/DirectX11/DX11VertexTransformPass.h"

#include "include/Renderer/DirectX11/Shaders/Compiled/TransformVertex.h"
#include "include/RenderQueue/RenderableCollection.h"

namespace JonsEngine
{
	enum VSInputLayout
	{
		POSITION = 0,
		BONE_INDEX,
		BONE_WEIGHT,
		NUM_INPUT_LAYOUTS
	};


    DX11VertexTransformPass::DX11VertexTransformPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, IDMap<DX11Mesh>& meshMap) :
        mContext(context), mTransformCBuffer(device, context, mTransformCBuffer.CONSTANT_BUFFER_SLOT_VERTEX), mMeshMap(meshMap)
    {
        D3D11_INPUT_ELEMENT_DESC inputDescription[NUM_INPUT_LAYOUTS];
        ZeroMemory(&inputDescription, sizeof(D3D11_INPUT_ELEMENT_DESC));
        inputDescription[VSInputLayout::POSITION].SemanticName = "POSITION";
        inputDescription[VSInputLayout::POSITION].SemanticIndex = 0;
        inputDescription[VSInputLayout::POSITION].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputDescription[VSInputLayout::POSITION].InputSlot = 0;
        inputDescription[VSInputLayout::POSITION].AlignedByteOffset = 0;
        inputDescription[VSInputLayout::POSITION].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription[VSInputLayout::POSITION].InstanceDataStepRate = 0;

		inputDescription[VSInputLayout::BONE_INDEX].SemanticName = "BONE_INDICES";
		inputDescription[VSInputLayout::BONE_INDEX].SemanticIndex = 0;
		inputDescription[VSInputLayout::BONE_INDEX].Format = DXGI_FORMAT_R8G8B8A8_UINT;
		inputDescription[VSInputLayout::BONE_INDEX].InputSlot = DX11Mesh::VERTEX_BUFFER_SLOT_BONE_WEIGHTS;
		inputDescription[VSInputLayout::BONE_INDEX].AlignedByteOffset = 0;
		inputDescription[VSInputLayout::BONE_INDEX].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputDescription[VSInputLayout::BONE_INDEX].InstanceDataStepRate = 0;

		inputDescription[VSInputLayout::BONE_WEIGHT].SemanticName = "BONE_WEIGHTS";
		inputDescription[VSInputLayout::BONE_WEIGHT].SemanticIndex = 0;
		inputDescription[VSInputLayout::BONE_WEIGHT].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputDescription[VSInputLayout::BONE_WEIGHT].InputSlot = DX11Mesh::VERTEX_BUFFER_SLOT_BONE_WEIGHTS;
		inputDescription[VSInputLayout::BONE_WEIGHT].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		inputDescription[VSInputLayout::BONE_WEIGHT].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputDescription[VSInputLayout::BONE_WEIGHT].InstanceDataStepRate = 0;

        DXCALL(device->CreateInputLayout(inputDescription, VSInputLayout::NUM_INPUT_LAYOUTS, gTransformVertexShader, sizeof(gTransformVertexShader), &mInputLayout));
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

    void DX11VertexTransformPass::RenderMesh(DX11Mesh& mesh, const Mat4& wvpMatrix, const bool isAnimating)
    {
        mTransformCBuffer.SetData(TransformCBuffer(wvpMatrix, isAnimating));
        mesh.DrawPositions();
    }

	void DX11VertexTransformPass::RenderMeshes(const RenderQueue::RenderData& renderData, const RenderableCollection& renderables, const Mat4& viewProjectionMatrix)
	{
		// static meshes
		const auto staticBegin = renderables.mStaticMeshesBegin;
		const auto staticEnd = renderables.mStaticMeshesEnd;
		const auto& staticMeshesContainer = renderData.mStaticMeshes;
		RenderMeshesAux(staticMeshesContainer, staticBegin, staticEnd, viewProjectionMatrix, false);

		// animated meshes
		const auto animatedBegin = renderables.mAnimatedMeshesBegin;
		const auto animatedEnd = renderables.mAnimatedMeshesEnd;
		const auto& animatedMeshesContainer = renderData.mAnimatedMeshes;
		RenderMeshesAux(animatedMeshesContainer, animatedBegin, animatedEnd, viewProjectionMatrix, true);
	}

	void DX11VertexTransformPass::RenderAABBs(const RenderQueue::RenderData& renderData, const RenderableCollection& renderables, const Mat4& viewProjectionMatrix)
	{
		// static meshes
		const auto staticBegin = renderables.mStaticMeshesBegin;
		const auto staticEnd = renderables.mStaticMeshesEnd;
		const auto& staticMeshesContainer = renderData.mStaticMeshes;
		RenderAABBsAux(staticMeshesContainer, staticBegin, staticEnd, viewProjectionMatrix);

		// animated meshes
		const auto animatedBegin = renderables.mAnimatedMeshesBegin;
		const auto animatedEnd = renderables.mAnimatedMeshesEnd;
		const auto& animatedMeshesContainer = renderData.mAnimatedMeshes;
		RenderAABBsAux(animatedMeshesContainer, animatedBegin, animatedEnd, viewProjectionMatrix);
	}


	void DX11VertexTransformPass::RenderMeshesAux(const RenderableMesh::ContainerType& meshContainer, const RenderableMesh::Index beginIndex, const RenderableMesh::Index endIndex, const Mat4& viewProjectionMatrix, const bool isAnimating)
	{
		for (auto meshIndex = beginIndex; meshIndex < endIndex; ++meshIndex)
		{
			const RenderableMesh& mesh = meshContainer.at(meshIndex);
			mTransformCBuffer.SetData(TransformCBuffer(viewProjectionMatrix * mesh.mWorldTransform, isAnimating));
			mMeshMap.GetItem(mesh.mMeshID).DrawPositions();
		}
	}

	void DX11VertexTransformPass::RenderAABBsAux(const RenderableMesh::ContainerType& meshContainer, const RenderableMesh::Index beginIndex, const RenderableMesh::Index endIndex, const Mat4& viewProjectionMatrix)
	{
		for (auto meshIndex = beginIndex; meshIndex < endIndex; ++meshIndex)
		{
			const RenderableMesh& mesh = meshContainer.at(meshIndex);
			mTransformCBuffer.SetData(TransformCBuffer(viewProjectionMatrix * mesh.mWorldTransform, false));
			mMeshMap.GetItem(mesh.mMeshID).DrawAABB();
		}
	}
}