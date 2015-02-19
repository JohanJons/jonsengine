#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Scene/ModelNode.h"
#include "include/Scene/Material.h"
#include "include/Resources/JonsPackage.h"
#include "include/Core/Types.h"
#include "include/Core/Utils/Math.h"

#include <string>
#include <vector>
#include <memory>

namespace JonsEngine
{
	class DX11Renderer;

    struct ModelMesh
    {
        ModelMesh(const MeshID meshID, const uint32_t aabbIndex, const uint32_t transformIndex);


        const MeshID mMeshID;

        const AABB* mAABBIndex;
        const Transform* mTransformIndex;
    };

    struct ModelNode
    {
        ModelNode(const uint32_t aabbIndex, const uint32_t transformIndex);


        const ModelNode* mNodeStart;
        const ModelNode* mNodeEnd;

        const ModelMesh* mMeshStart;
        const ModelMesh* mMeshEnd;

        const AABB* mAABB;
        const Transform* mTransform;
    };

    struct ModelDataCollection
    {
        std::vector<AABB> mAABBs;
        std::vector<Mat4> mTransforms;
        std::vector<ModelMesh> mMeshes;
        std::vector<ModelNode> mNodes;
    };

    struct Model
    {
		Model(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds, const MeshID meshID, MaterialPtr material);
        Model(DX11Renderer& renderer, const JonsPackagePtr jonsPkg, const PackageModel& pkgModel, LoadMaterialFunc loadMaterialFunction, ModelDataCollection& modelData);
        ~Model();

		bool operator==(const Model& m);
        bool operator==(const std::string& modelName);


		const std::string mName;
        const size_t mHashedID;

        const ModelNode* mRootNode;
    };

	typedef std::shared_ptr<Model> ModelPtr;
}