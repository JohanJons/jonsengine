#include "include/RenderQueue/RenderableMesh.h"

namespace JonsEngine
{
	RenderableMesh::RenderableMesh(const Mat4& localWorldTransform, const MaterialIndex materialIndex, const float materialTilingFactor, const DX11MeshID meshID) :
		mLocalWorldTransform(localWorldTransform),
		mMaterial(materialIndex),
		mMaterialTilingFactor(materialTilingFactor),
		mMeshID(meshID)
	{
	}

	RenderableMesh::RenderableMesh(const Mat4& localWorldTransform, const BoneIndexRange& boneRange, const MaterialIndex materialIndex, const float materialTilingFactor, const DX11MeshID meshID) :
		mLocalWorldTransform(localWorldTransform),
		mSkeleton(boneRange),
		mMaterial(materialIndex),
		mMaterialTilingFactor(materialTilingFactor),
		mMeshID(meshID)
	{
	}
}