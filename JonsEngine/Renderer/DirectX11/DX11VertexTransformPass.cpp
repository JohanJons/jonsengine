#include "DX11VertexTransformPass.h"

#include "Shaders/Compiled/TransformStaticVertex.h"
#include "Shaders/Compiled/TransformStaticInstancedVertex.h"
#include "Shaders/Compiled/TransformAnimatedVertex.h"
#include "Shaders/Constants.h"

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
		mInstancedDataBuffer(device, context),
		mTransformCBuffer(device, context, mTransformCBuffer.CONSTANT_BUFFER_SLOT_VERTEX),
		mMeshMap(meshMap),

		mContext(context),
		mStaticShader(nullptr),
		mStaticInstancedShader(nullptr),
		mAnimatedShader(nullptr),
		mLayoutStatic(nullptr),
		mLayoutAnimated(nullptr)
    {
		// static layout
        D3D11_INPUT_ELEMENT_DESC inputDescStatic[StaticLayout::STATIC_NUM_INPUT_LAYOUTS];
        ZeroMemory(&inputDescStatic, sizeof(D3D11_INPUT_ELEMENT_DESC));
		inputDescStatic[StaticLayout::STATIC_POSITION].SemanticName = "POSITION";
		inputDescStatic[StaticLayout::STATIC_POSITION].SemanticIndex = 0;
		inputDescStatic[StaticLayout::STATIC_POSITION].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputDescStatic[StaticLayout::STATIC_POSITION].InputSlot = DX11Mesh::VERTEX_BUFFER_SLOT_POSITIONS;
		inputDescStatic[StaticLayout::STATIC_POSITION].AlignedByteOffset = 0;
		inputDescStatic[StaticLayout::STATIC_POSITION].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputDescStatic[StaticLayout::STATIC_POSITION].InstanceDataStepRate = 0;
		DXCALL(device->CreateInputLayout(inputDescStatic, StaticLayout::STATIC_NUM_INPUT_LAYOUTS, gTransformStaticVertexShader, sizeof(gTransformStaticVertexShader), &mLayoutStatic));
		DXCALL(device->CreateVertexShader(gTransformStaticVertexShader, sizeof(gTransformStaticVertexShader), nullptr, &mStaticShader));
		DXCALL(device->CreateVertexShader(gTransformStaticInstancedVertexShader, sizeof(gTransformStaticInstancedVertexShader), nullptr, &mStaticInstancedShader));

		// animated layout
		D3D11_INPUT_ELEMENT_DESC inputDescAnimated[AnimatedLayout::ANIMATED_NUM_INPUT_LAYOUTS];
		ZeroMemory(&inputDescAnimated, sizeof(D3D11_INPUT_ELEMENT_DESC));
		inputDescAnimated[AnimatedLayout::ANIMATED_POSITION].SemanticName = "POSITION";
		inputDescAnimated[AnimatedLayout::ANIMATED_POSITION].SemanticIndex = 0;
		inputDescAnimated[AnimatedLayout::ANIMATED_POSITION].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputDescAnimated[AnimatedLayout::ANIMATED_POSITION].InputSlot = DX11Mesh::VERTEX_BUFFER_SLOT_POSITIONS;
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
		RenderStaticMesh(mesh, wvpMatrix, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

    void DX11VertexTransformPass::RenderStaticMesh(DX11Mesh& mesh, const Mat4& wvpMatrix, const D3D_PRIMITIVE_TOPOLOGY topology)
    {
		BindForStaticRendering(StaticRenderMode::NonInstanced, topology);

		const uint32_t noBoneIndexOffset = 0;
        mTransformCBuffer.SetData(TransformCBuffer(wvpMatrix, noBoneIndexOffset));
		mTransformCBuffer.Bind();
        mesh.DrawPositions();
    }

	void DX11VertexTransformPass::RenderStaticMeshInstanced(DX11Mesh& mesh, const Mat4& viewProjectionMatrix, const std::vector<Mat4>& worldTransforms)
	{
		RenderStaticMeshInstanced(mesh, viewProjectionMatrix, worldTransforms, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void DX11VertexTransformPass::RenderStaticMeshInstanced(DX11Mesh& mesh, const Mat4& viewProjectionMatrix, const std::vector<Mat4>& worldTransforms, D3D_PRIMITIVE_TOPOLOGY topology)
	{
		BindForStaticRendering(StaticRenderMode::Instanced, topology);

		mInstancedDataBuffer.SetData( worldTransforms );
		mInstancedDataBuffer.Bind(DX11CPUDynamicBuffer::Shaderslot::Vertex, SBUFFER_SLOT_EXTRA);

		const uint32_t noBoneIndexOffset = 0;
		mTransformCBuffer.SetData(TransformCBuffer(viewProjectionMatrix, noBoneIndexOffset));
		mTransformCBuffer.Bind();

		const uint32_t numTransforms = worldTransforms.size();
		assert(numTransforms > 0);
		mesh.DrawPositionsInstanced(numTransforms);
	}

	void DX11VertexTransformPass::RenderStaticMeshes(const RenderableMeshContainer& renderData, const MeshIndex start, const MeshIndex stop, const Mat4& viewProjectionMatrix)
	{
		RenderStaticMeshes(renderData, start, stop, viewProjectionMatrix, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void DX11VertexTransformPass::RenderStaticMeshes(const RenderableMeshContainer& renderData, const MeshIndex start, const MeshIndex stop, const Mat4& viewProjectionMatrix, const D3D_PRIMITIVE_TOPOLOGY topology)
	{
		BindForStaticRendering(StaticRenderMode::NonInstanced, topology);
		RenderMeshesAux(renderData, start, stop, viewProjectionMatrix);
	}

	void DX11VertexTransformPass::RenderAnimatedMeshes(const RenderableMeshContainer& renderData, const MeshIndex start, const MeshIndex stop, const Mat4& viewProjectionMatrix)
	{
		RenderAnimatedMeshes(renderData, start, stop, viewProjectionMatrix, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void DX11VertexTransformPass::RenderAnimatedMeshes(const RenderableMeshContainer& renderData, const MeshIndex start, const MeshIndex stop, const Mat4& viewProjectionMatrix, const D3D_PRIMITIVE_TOPOLOGY topology)
	{
		BindForAnimatedRendering(topology);
		RenderMeshesAux(renderData, start, stop, viewProjectionMatrix);
	}

	void DX11VertexTransformPass::RenderMeshes(const RenderData& renderData, const RenderableCollection& renderables, const Mat4& viewProjectionMatrix)
	{
		RenderMeshes(renderData, renderables, viewProjectionMatrix, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void DX11VertexTransformPass::RenderMeshes(const RenderData& renderData, const RenderableCollection& renderables, const Mat4& viewProjectionMatrix, const D3D_PRIMITIVE_TOPOLOGY topology)
	{
		RenderStaticMeshes(renderData.mStaticMeshes, renderables.mStaticMeshesBegin, renderables.mStaticMeshesEnd, viewProjectionMatrix, topology);
		RenderAnimatedMeshes(renderData.mAnimatedMeshes, renderables.mAnimatedMeshesBegin, renderables.mAnimatedMeshesEnd, viewProjectionMatrix, topology);
	}

	// TODO: should be refactored into something that uses instanced drawing rather than several draw calls
	void DX11VertexTransformPass::RenderAABBs( const std::vector<AABBRenderData>& aabbRenderData, const Mat4& viewProjMatrix )
	{
		// only use static rendering, since AABBs are enlarged enough to cover all poses anyway
		BindForStaticRendering(StaticRenderMode::NonInstanced, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		// TODO: separate cbuffer for boneindex
		const uint32_t noBoneIndexOffset = 0;
		for ( const AABBRenderData& AABBData : aabbRenderData )
		{
			const Mat4& worldTransform = AABBData.first;
			const DX11MeshID meshID = AABBData.second;

			mTransformCBuffer.SetData( TransformCBuffer( viewProjMatrix * worldTransform, noBoneIndexOffset ) );
			mTransformCBuffer.Bind();
			mMeshMap.GetItem( meshID ).DrawAABB();
		}
	}


	void DX11VertexTransformPass::BindForStaticRendering(const StaticRenderMode mode, const D3D_PRIMITIVE_TOPOLOGY topology)
	{
		mContext->IASetInputLayout(mLayoutStatic);
		mContext->IASetPrimitiveTopology(topology);
		if (mode == StaticRenderMode::NonInstanced)
			mContext->VSSetShader(mStaticShader, nullptr, 0);
		else
			mContext->VSSetShader(mStaticInstancedShader, nullptr, 0);
	}

	void DX11VertexTransformPass::BindForAnimatedRendering(const D3D_PRIMITIVE_TOPOLOGY topology)
	{
		mContext->IASetInputLayout(mLayoutAnimated);
		mContext->IASetPrimitiveTopology(topology);
		mContext->VSSetShader(mAnimatedShader, nullptr, 0);
	}

	void DX11VertexTransformPass::RenderMeshesAux(const RenderableMesh::ContainerType& meshContainer, const RenderableMesh::Index beginIndex, const RenderableMesh::Index endIndex, const Mat4& viewProjectionMatrix)
	{
		for (auto meshIndex = beginIndex; meshIndex < endIndex; ++meshIndex)
		{
			const RenderableMesh& mesh = meshContainer.at(meshIndex);

			uint32_t boneOffset = 0;
			const bool hasBones = mesh.mSkeleton.mBoneRange.first != INVALID_BONE_INDEX;
			if (hasBones)
				boneOffset = mesh.mSkeleton.mBoneRange.first;

			mTransformCBuffer.SetData(TransformCBuffer(viewProjectionMatrix * mesh.mLocalWorldTransform, boneOffset));
			mTransformCBuffer.Bind();
			mMeshMap.GetItem(mesh.mMeshID).DrawPositions();
		}
	}
}