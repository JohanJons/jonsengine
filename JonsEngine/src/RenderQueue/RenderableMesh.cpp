#include "include/RenderQueue/RenderableMesh.h"

namespace JonsEngine
{
	RenderableMesh::RenderableMesh(const Mat4& worldTransform, const MaterialIndex materialIndex, const float materialTilingFactor, const DX11MeshID meshID) :
		mWorldTransform(worldTransform),
		mSkeleton(RenderableBone::INVALID_INDEX, RenderableBone::INVALID_INDEX),
		mMaterial(materialIndex),
		mMaterialTilingFactor(materialTilingFactor),
		mMeshID(meshID)
	{
	}

	RenderableMesh::RenderableMesh(const Mat4& worldTransform, const BoneIndexRange& boneRange, const MaterialIndex materialIndex, const float materialTilingFactor, const DX11MeshID meshID) :
		mWorldTransform(worldTransform),
		mSkeleton(beginBone, endBone),
		mMaterial(materialIndex),
		mMaterialTilingFactor(materialTilingFactor),
		mMeshID(meshID)
	{
	}
}