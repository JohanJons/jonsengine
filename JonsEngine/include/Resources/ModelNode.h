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

        typedef NodeContainer::const_iterator NodeIterator;
        typedef ConstRangedIterator<MeshContainer> MeshIterator;
        typedef ConstRangedIterator<NodeContainer> AllChildrenIterator;
        
        class ImmediateChildrenIter : public NodeIterator
        {
        public:
            inline ImmediateChildrenIter(const NodeIterator& iter) : NodeIterator(iter)
            {
            }

            inline ImmediateChildrenIter& operator++()
            {
                return *this;
            }
        };

        typedef ConstRangedIterator<NodeContainer, ImmediateChildrenIter> ImmediateChildrenIterator;

        /*class ImmediateChildrenIterator
        {
        public:
            class Iterator
            {
            public:
                Iterator(ModelNode* item);
                ~Iterator();

                bool operator!=(const Iterator& iterator) const;
                Iterator& operator++();
                const ModelNode& operator*() const;


            private:
                ModelNode* mItem;
            };

            ImmediateChildrenIterator(ModelNode& begin, ModelNode& end);
            ~ImmediateChildrenIterator();

            Iterator begin() const;
            Iterator end() const;


        private:
            ModelNode& mBegin;
            ModelNode& mEnd;
        };*/

        /*class AllChildrenIterator
        {
        public:
            class Iterator
            {
            public:
                Iterator(ModelNode* item);
                ~Iterator();

                bool operator!=(const Iterator& iterator) const;
                Iterator& operator++();
                const ModelNode& operator*() const;


            private:
                ModelNode* mItem;
            };

            AllChildrenIterator(ModelNode& begin, ModelNode& end);
            ~AllChildrenIterator();

            Iterator begin() const;
            Iterator end() const;


        private:
            ModelNode& mBegin;
            ModelNode& mEnd;
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


    private:
        ModelNode* mNext;
    };
}