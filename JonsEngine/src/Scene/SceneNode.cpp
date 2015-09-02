#include "include/Scene/SceneNode.h"
#include "include/Core/Math/Math.h"
#include "include/Core/Memory/HeapAllocator.h"

#include <algorithm>

namespace JonsEngine
{
    SceneNode::SceneNode(const std::string& nodeName, const OnSceneNodeDirtyFunc& onDirty) :
        mName(nodeName), mWorldTransform(gIdentityMatrix), mOrientation(1.0f, 0.0f, 0.0f, 0.0f), mScale(1.0f), mTranslation(0.0f), mOnDirtyFunc(onDirty)
    {
    }
        
    SceneNode::~SceneNode()
    {
    }

        
    void SceneNode::ScaleNode(const float scale)
    { 
        mScale *= scale;

        mOnDirtyFunc(this);
    }

    void SceneNode::TranslateNode(const Vec3& translateVec)
    { 
        mTranslation += translateVec;

        mOnDirtyFunc(this);
    }

    void SceneNode::RotateNode(const float angle, const Vec3& rotateVec)       
    { 
        mOrientation = glm::rotate(mOrientation, glm::radians(angle), rotateVec);

        mOnDirtyFunc(this);
    }


    void SceneNode::UpdateWorldMatrix()
    {
        UpdateTransform();

        for (SceneNodePtr childNode : mChildNodes)
            childNode->UpdateChildren(mTransform.Get());
    }


    Vec3 SceneNode::Position() const
    {
        Vec3 position(0.0f);
        position += mTranslation;

        return position;
    }


    void SceneNode::UpdateTransform()
    {
        mWorldTransform = glm::translate(gIdentityMatrix, mTranslation);
        mWorldTransform *= glm::toMat4(mOrientation);
        mWorldTransform = glm::scale(mWorldTransform, mScale);
    }

    void SceneNode::UpdateChildren(const Mat4& parentModelMatrix)
    {
        UpdateTransform();

        Mat4& worldMatrix = mTransform.Get();
        worldMatrix = parentModelMatrix * worldMatrix;

        for (SceneNodePtr childNode : mChildNodes)
            childNode->UpdateChildren(worldMatrix);
    }
}
