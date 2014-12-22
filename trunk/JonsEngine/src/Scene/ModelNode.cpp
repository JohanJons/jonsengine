#include "include/Scene/ModelNode.h"

#include "include/Core/Utils/Math.h"

namespace JonsEngine
{
    ModelNode::ModelNode(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds) :
        mName(name), mTransform(initialTransform), mAABBCenter(0.5f * (minBounds + maxBounds)), mAABBExtent(0.5f * (maxBounds - minBounds))
    {
    }

    ModelNode::~ModelNode()
    {
    }


    void ModelNode::Transform(const Mat4& transformMatrix)
    {
        mTransform *= transformMatrix;
    }


    const Mat4& ModelNode::GetTransformMatrix() const
    {
        return mTransform;
    }

    const Vec3& ModelNode::GetAABBCenter() const
    {
        return mAABBCenter;
    }

    const Vec3& ModelNode::GetAABBExtent() const
    {
        return mAABBExtent;
    }
}