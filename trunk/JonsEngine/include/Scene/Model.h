#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Scene/ModelNode.h"
#include "include/Scene/Material.h"
#include "include/Resources/JonsPackage.h"
#include "include/Core/Types.h"

#include <string>
#include <vector>
#include <memory>

namespace JonsEngine
{
	class DX11Renderer;

    //struct ModelMesh
    //{
    //    typedef std::vector<ModelMesh>::size_type Index;

    //    ModelMesh(const MeshID meshID, MaterialPtr material, const AABB::Index aabbIndex, const Transform::Index transformIndex);


    //    const MeshID mMeshID;

    //    MaterialPtr mMaterial;

    //    const AABB::Index mAABBIndex;
    //    const Transform::Index mTransformIndex;
    //};

    //struct ModelNode
    //{
    //    typedef std::vector<ModelNode>::size_type Index;

    //    ModelNode(const ModelNode::Index childrenBegin, const ModelNode::Index childrenEnd, const ModelMesh::Index meshesBegin, const ModelMesh::Index meshesEnd, const AABB::Index aabb, const Transform::Index transform);

    //    // '-end' is the index past the end of the range
    //    const ModelNode::Index mChildrenBegin;
    //    const ModelNode::Index mChildrenEnd;

    //    // see comment about model indices above
    //    const ModelMesh::Index mMeshesBegin;
    //    const ModelMesh::Index mMeshesEnd;

    //    const AABB::Index mAABB;
    //    const Transform::Index mTransform;
    //};

    class Model
    {
    public:
		Model(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds, const MeshID meshID, MaterialPtr material);
        Model(DX11Renderer& renderer, const JonsPackagePtr jonsPkg, const PackageModel& pkgModel, LoadMaterialFunc loadMaterialFunction);
        ~Model();

		bool operator==(const Model& m);
        bool operator==(const std::string& modelName);

		ModelNode& GetRootNode();


		const std::string mName;

        
    private:
        const size_t mHashedID;
		ModelNode mRootNode;
    };

	typedef std::shared_ptr<Model> ModelPtr;
}