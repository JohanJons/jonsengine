#pragma once

#include "include/RenderQueue/RenderableContainer.h"
#include "include/RenderQueue/RenderableSkeleton.h"
#include "include/RenderQueue/RenderableMaterial.h"
#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
    struct RenderableMesh : RenderableContainer<RenderableMesh>
    {
		typedef RenderableMaterial::Index MaterialIndex;
		typedef RenderableSkeleton::BoneIndex BoneIndex;

        RenderableMesh(const Mat4& worldTransform, const MaterialIndex materialIndex, const float materialTilingFactor, const DX11MeshID meshID);
		RenderableMesh(const Mat4& worldTransform, const BoneIndex beginBone, const BoneIndex endBone, const MaterialIndex materialIndex, const float materialTilingFactor, const DX11MeshID meshID);


        Mat4 mWorldTransform;
        RenderableSkeleton mSkeleton;
		MaterialIndex mMaterial;
        float mMaterialTilingFactor;
        DX11MeshID mMeshID;
    };
}