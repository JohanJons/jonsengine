#pragma once

#include "include/Resources/ModelNode.h"
#include "include/Resources/Mesh.h"
#include "include/Resources/JonsPackage.h"
#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.hpp"

#include <string>
#include <vector>

namespace JonsEngine
{
    class DX11Renderer;

    class Model
    {
    public:
        Model(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds, const DX11MeshID meshID);
        Model(const PackageModel& pkgModel, const ModelNode::InitDataList& initData);
        ~Model();

        const ModelNode& GetRootNode() const;


        const std::string mName;


    private:
        typedef std::vector<ModelNode> NodeContainer;
        typedef std::vector<Mesh> MeshContainer;

        void ParseNodes(const ModelNode::InitDataList& initDataList, const PackageNode& pkgNode, const ModelNode::NodeIterator& next);
        ModelNode::MeshIterator ParseMeshes(const ModelNode::InitDataList& initDataList, const PackageNode& pkgNode);


        NodeContainer mNodes;
        MeshContainer mMeshes;
    };

    typedef IDMap<Model>::ItemID ModelID;
    static const ModelID INVALID_MODEL_ID = IDMap<Model>::INVALID_ITEM_ID;
}