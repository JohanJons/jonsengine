#pragma once

#include "Resources/Mesh.h"
#include "Resources/JonsPackage.h"
#include "Core/Containers/RangedIterator.hpp"
#include "Core/Types.h"
#include "Core/Math/AABB.h"

#include <vector>
#include <string>
#include <tuple>
#include <limits>

namespace JonsEngine
{
    typedef PackageNode::NodeIndex ModelNodeIndex;
    static const ModelNodeIndex INVALID_MODEL_NODE_INDEX = PackageNode::INVALID_NODE_INDEX;

    class ModelNode
    {
    public:
        typedef std::tuple<const PackageMesh&, const DX11MeshID, const MaterialID> InitData;
        typedef std::vector<InitData> InitDataList;

        typedef std::vector<Mesh> MeshContainer;
        typedef ConstRangedIterator<MeshContainer> MeshIterator;

    public:
        ModelNode( const PackageNode& pkgNode, const MeshIterator& meshIter );
        ModelNode( const std::string& name, const ModelNodeIndex nodeID, const Mat4& localTransform, const MeshIterator& meshIter );

        uint32_t GetNumMeshes() const;
        const std::string& GetName() const;
        ModelNodeIndex GetModelNodeIndex() const;
		const Mat4& GetLocalTransform() const;

        MeshIterator GetMeshes() const;

    private:
        std::string mName;
        ModelNodeIndex mNodeIndex;
		Mat4 mLocalTransform;
        uint32_t mNumMeshes;

        MeshIterator mMeshes;
    };
}