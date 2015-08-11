#include "include/Resources/Model.h"

namespace JonsEngine
{
    Model::Model(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds, const DX11MeshID meshID) : mName(name)
    {
        mNodes.emplace_back(mNodes, mMeshes, name, minBounds, maxBounds, initialTransform, meshID);
    }

    Model::Model(const PackageModel& pkgModel, const ModelNode::InitDataList& initData) : mName(pkgModel.mName)
    {
        mNodes.emplace_back(pkgModel.mRootNode, initData, mNodes, mMeshes);
    }

    Model::~Model()
    {
    }


    const ModelNode& Model::GetRootNode() const
    {
        return mNodes.front();
    }
}