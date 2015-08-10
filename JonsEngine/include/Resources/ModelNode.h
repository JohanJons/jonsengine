#pragma once

#include "include/Resources/Mesh.h"
#include "include/Resources/JonsPackage.h"
#include "include/Core/Containers/RangedIterator.hpp"
#include "include/Core/Types.h"
#include "include/Core/Math/AABB.h"

#include <vector>
#include <string>
#include <tuple>

namespace JonsEngine
{
    class ModelNode
    {
    public:
        typedef std::tuple<const PackageMesh&, const DX11MeshID, const MaterialID> InitData;
        typedef std::vector<InitData> InitDataList;

        typedef std::vector<ModelNode> NodeContainer;
        typedef std::vector<Mesh> MeshContainer;

        typedef ConstRangedIterator<MeshContainer> MeshIterator;
        //typedef ConstRangedIterator<NodeContainer> NodeIterator;
        
        /*class ImmediateChildrenIterator
        {
        public:
            typedef NodeContainer::iterator Iterator;

            bool operator!=(const Iterator& iterator) const;
            Iterator& operator++();
            Iterator operator++(int);
            ModelNode& operator*();
            const ModelNode& operator*() const;

            Iterator begin();
            Iterator end();
        };

        class AllChildrenIterator
        {
        public:
            typedef NodeContainer::iterator Iterator;

            Iterator begin();
            Iterator end();
        };*/

        ModelNode(const PackageNode& pkgNode, const InitDataList& meshResources, NodeContainer& nodes, MeshContainer& meshes);
        ModelNode(NodeContainer& nodes, MeshContainer& meshes, const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const Mat4& initialTransform, const DX11MeshID meshID);
        ~ModelNode();


        const std::string mName;
        const AABB mLocalAABB;
        const Mat4 mLocalTransform;

        const ImmediateChildrenIterator mImmediateChildNodes;
        const AllChildrenIterator mAllChildNodes;
        const MeshIterator mMeshes;
    };
}