#include "include/RenderQueue/RenderableMesh.h"

namespace JonsEngine
{
	RenderableMesh::RenderableMesh(const Mat4& worldTransform, const MaterialIndex materialIndex, const float materialTilingFactor, const DX11MeshID meshID) :
		mWorldTransform(worldTransform),
		mSkeleton(INVALID_BONE_INDEX, INVALID_BONE_INDEX),
		mMaterial(materialIndex),
		mMaterialTilingFactor(materialTilingFactor),
		mMeshID(meshID)
	{
	}

	RenderableMesh::RenderableMesh(const Mat4& worldTransform, const BoneIndexRange& boneRange, const MaterialIndex materialIndex, const float materialTilingFactor, const DX11MeshID meshID) :
		mWorldTransform(worldTransform),
		mSkeleton(boneRange),
		mMaterial(materialIndex),
		mMaterialTilingFactor(materialTilingFactor),
		mMeshID(meshID)
	{
	}
}