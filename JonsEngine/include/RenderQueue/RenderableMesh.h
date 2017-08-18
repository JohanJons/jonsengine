#pragma once

#include "include/RenderQueue/RenderableContainer.h"
#include "include/RenderQueue/RenderableMaterial.h"
#include "include/RenderQueue/RenderableSkeleton.h"
#include "include/Resources/Bone.h"
#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
	struct RenderableMesh : RenderableContainer<RenderableMesh>
    {
		RenderableMesh(const Mat4& localWorldTransform, const MaterialIndex materialIndex, const float materialTilingFactor, const DX11MeshID meshID);
		RenderableMesh(const Mat4& localWorldTransform, const BoneIndexRange& boneRange, const MaterialIndex materialIndex, const float materialTilingFactor, const DX11MeshID meshID);


        Mat4 mLocalWorldTransform;
		RenderableSkeleton mSkeleton;
		MaterialIndex mMaterial;
        float mMaterialTilingFactor;
        DX11MeshID mMeshID;
    };

	typedef RenderableMesh::Index MeshIndex;
	typedef RenderableMesh::ContainerType RenderableMeshContainer;
}