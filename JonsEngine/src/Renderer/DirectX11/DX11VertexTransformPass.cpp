#include "include/Renderer/DirectX11/DX11VertexTransformPass.h"

#include "include/Renderer/DirectX11/Shaders/Compiled/TransformStaticVertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/TransformAnimatedVertex.h"
#include "include/RenderQueue/RenderableCollection.h"

namespace JonsEngine
{
	enum StaticLayout
	{
		STATIC_POSITION = 0,
		STATIC_NUM_INPUT_LAYOUTS
	};

	enum AnimatedLayout
	{
		ANIMATED_POSITION = 0,
		ANIMATED_BONE_INDEX,
		ANIMATED_BONE_WEIGHT,
		ANIMATED_NUM_INPUT_LAYOUTS
	};


	DX11VertexTransformPass::DX11VertexTransformPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, IDMap<DX11Mesh>& meshMap) :
		mContext(context),
		mStaticShader(nullptr),
		mAnimatedShader(nullptr),
		mLayoutStatic(nullptr),
		mLayoutAnimated(nullptr),
		mTransformCBuffer(device, context, mTransformCBuffer.CONSTANT_BUFFER_SLOT_VERTEX),
		mMeshMap(meshMap)
    {
		// static layout
        D3D11_INPUT_ELEMENT_DESC inputDescStatic[StaticLayout::STATIC_NUM_INPUT_LAYOUTS];
        ZeroMemory(&inputDescStatic, sizeof(D3D11_INPUT_ELEMENT_DESC));
		inputDescStatic[StaticLayout::STATIC_POSITION].SemanticName = "POSITION";
		inputDescStatic[StaticLayout::STATIC_POSITION].SemanticIndex = 0;
		inputDescStatic[StaticLayout::STATIC_POSITION].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputDescStatic[StaticLayout::STATIC_POSITION].InputSlot = 0;
		inputDescStatic[StaticLayout::STATIC_POSITION].AlignedByteOffset = 0;
		inputDescStatic[StaticLayout::STATIC_POSITION].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputDescStatic[StaticLayout::STATIC_POSITION].InstanceDataStepRate = 0;
		DXCALL(device->CreateInputLayout(inputDescStatic, StaticLayout::STATIC_NUM_INPUT_LAYOUTS, gTransformStaticVertexShader, sizeof(gTransformStaticVertexShader), &mLayoutStatic));
		DXCALL(device->CreateVertexShader(gTransformStaticVertexShader, sizeof(gTransformStaticVertexShader), nullptr, &mStaticShader));

		// animated layout
		D3D11_INPUT_ELEMENT_DESC inputDescAnimated[AnimatedLayout::ANIMATED_NUM_INPUT_LAYOUTS];
		ZeroMemory(&inputDescAnimated, sizeof(D3D11_INPUT_ELEMENT_DESC));
		inputDescAnimated[AnimatedLayout::ANIMATED_POSITION].SemanticName = "POSITION";
		inputDescAnimated[AnimatedLayout::ANIMATED_POSITION].SemanticIndex = 0;
		inputDescAnimated[AnimatedLayout::ANIMATED_POSITION].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputDescAnimated[AnimatedLayout::ANIMATED_POSITION].InputSlot = 0;
		inputDescAnimated[AnimatedLayout::ANIMATED_POSITION].AlignedByteOffset = 0;
		inputDescAnimated[AnimatedLayout::ANIMATED_POSITION].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputDescAnimated[AnimatedLayout::ANIMATED_POSITION].InstanceDataStepRate = 0;

		inputDescAnimated[AnimatedLayout::ANIMATED_BONE_INDEX].SemanticName = "BONE_INDICES";
		inputDescAnimated[AnimatedLayout::ANIMATED_BONE_INDEX].SemanticIndex = 0;
		inputDescAnimated[AnimatedLayout::ANIMATED_BONE_INDEX].Format = DXGI_FORMAT_R8G8B8A8_UINT;
		inputDescAnimated[AnimatedLayout::ANIMATED_BONE_INDEX].InputSlot = DX11Mesh::VERTEX_BUFFER_SLOT_BONE_WEIGHTS;
		inputDescAnimated[AnimatedLayout::ANIMATED_BONE_INDEX].AlignedByteOffset = 0;
		inputDescAnimated[AnimatedLayout::ANIMATED_BONE_INDEX].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputDescAnimated[AnimatedLayout::ANIMATED_BONE_INDEX].InstanceDataStepRate = 0;

		inputDescAnimated[AnimatedLayout::ANIMATED_BONE_WEIGHT].SemanticName = "BONE_WEIGHTS";
		inputDescAnimated[AnimatedLayout::ANIMATED_BONE_WEIGHT].SemanticIndex = 0;
		inputDescAnimated[AnimatedLayout::ANIMATED_BONE_WEIGHT].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputDescAnimated[AnimatedLayout::ANIMATED_BONE_WEIGHT].InputSlot = DX11Mesh::VERTEX_BUFFER_SLOT_BONE_WEIGHTS;
		inputDescAnimated[AnimatedLayout::ANIMATED_BONE_WEIGHT].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		inputDescAnimated[AnimatedLayout::ANIMATED_BONE_WEIGHT].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputDescAnimated[AnimatedLayout::ANIMATED_BONE_WEIGHT].InstanceDataStepRate = 0;
        DXCALL(device->CreateInputLayout(inputDescAnimated, AnimatedLayout::ANIMATED_NUM_INPUT_LAYOUTS, gTransformAnimatedVertexShader, sizeof(gTransformAnimatedVertexShader), &mLayoutAnimated));
        DXCALL(device->CreateVertexShader(gTransformAnimatedVertexShader, sizeof(gTransformAnimatedVertexShader), nullptr, &mAnimatedShader));
    }

    DX11VertexTransformPass::~DX11VertexTransformPass()
    {
    }


    void DX11VertexTransformPass::RenderStaticMesh(DX11Mesh& mesh, const Mat4& wvpMatrix)
    {
		mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		BindForStaticRendering();

        mTransformCBuffer.SetData(TransformCBuffer(wvpMatrix));
        mesh.DrawPositions();
    }

	void DX11VertexTransformPass::RenderMeshes(const RenderQueue::RenderData& renderData, const RenderableCollection& renderables, const Mat4& viewProjectionMatrix)
	{
		mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// static meshes
		BindForStaticRendering();
		const auto staticBegin = renderables.mStaticMeshesBegin;
		const auto staticEnd = renderables.mStaticMeshesEnd;
		const auto& staticMeshesContainer = renderData.mStaticMeshes;
		RenderMeshesAux(staticMeshesContainer, staticBegin, staticEnd, viewProjectionMatrix);

		// animated meshes
		BindForAnimatedRendering();
		const auto animatedBegin = renderables.mAnimatedMeshesBegin;
		const auto animatedEnd = renderables.mAnimatedMeshesEnd;
		const auto& animatedMeshesContainer = renderData.mAnimatedMeshes;
		RenderMeshesAux(animatedMeshesContainer, animatedBegin, animatedEnd, viewProjectionMatrix);
	}

	void DX11VertexTransformPass::RenderAABBs(const AABBRenderData& aabbRenderData)
	{
		mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		// only use static rendering, since AABBs are enlarged enough to cover all poses anyway
		BindForStaticRendering();

		const Mat4& viewProjMatrix = aabbRenderData.mCameraViewProjectionMatrix;
		for (const auto& aabbData : aabbRenderData.mRenderableAABBs)
		{
			const Mat4& worldTransform = aabbData.first;
			const DX11MeshID meshID = aabbData.second;

			mTransformCBuffer.SetData(TransformCBuffer(viewProjMatrix * worldTransform));
			mMeshMap.GetItem(meshID).DrawAABB();
		}
	}


	void DX11VertexTransformPass::BindForStaticRendering()
	{
		mContext->IASetInputLayout(mLayoutStatic);
		mContext->VSSetShader(mStaticShader, nullptr, 0);
	}

	void DX11VertexTransformPass::BindForAnimatedRendering()
	{
		mContext->IASetInputLayout(mLayoutAnimated);
		mContext->VSSetShader(mAnimatedShader, nullptr, 0);
	}

	void DX11VertexTransformPass::RenderMeshesAux(const RenderableMesh::ContainerType& meshContainer, const RenderableMesh::Index beginIndex, const RenderableMesh::Index endIndex, const Mat4& viewProjectionMatrix)
	{
		for (auto meshIndex = beginIndex; meshIndex < endIndex; ++meshIndex)
		{
			const RenderableMesh& mesh = meshContainer.at(meshIndex);
			mTransformCBuffer.SetData(TransformCBuffer(viewProjectionMatrix * mesh.mLocalWorldTransform));
			mMeshMap.GetItem(mesh.mMeshID).DrawPositions();
		}
	}
}