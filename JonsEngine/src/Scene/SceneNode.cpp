#include "include/Scene/SceneNode.h"
#include "include/Core/Math/Math.h"
#include "include/Core/Memory/HeapAllocator.h"

namespace JonsEngine
{
    SceneNode::SceneNode(const std::string& nodeName, const SceneNodeID parent, const OnSceneNodeDirtyFunc& onDirty) :
        mName(nodeName), mWorldTransform(gIdentityMatrix), mOrientation(1.0f, 0.0f, 0.0f, 0.0f), mScale(1.0f), mTranslation(0.0f), mParent(parent), mOnDirtyFunc(onDirty)
    {
    }

    SceneNode& SceneNode::operator=(SceneNode&& otherSceneNode)
    {
        if (this != &otherSceneNode)
        {
            mName = std::move(otherSceneNode.mName);
            mWorldTransform = std::move(otherSceneNode.mWorldTransform);
            mOrientation = std::move(otherSceneNode.mOrientation);
            mScale = std::move(otherSceneNode.mScale);
            mTranslation = std::move(otherSceneNode.mTranslation);

            mParent = std::move(otherSceneNode.mParent);
            mOnDirtyFunc = std::move(otherSceneNode.mOnDirtyFunc);

            otherSceneNode.mParent = INVALID_SCENE_NODE_ID;
        }

        return *this;
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


    void SceneNode::UpdateWorldMatrix(const Mat4& parentMatrix)
    {
        mWorldTransform = glm::translate(gIdentityMatrix, mTranslation);
        mWorldTransform *= glm::toMat4(mOrientation);
        mWorldTransform = glm::scale(mWorldTransform, mScale);
        mWorldTransform = parentMatrix * mWorldTransform;
    }


    const Vec3& SceneNode::Position() const
    {
        return mTranslation;
    }

    const Vec3& SceneNode::Scale() const
    {
        return mScale;
    }

    const Mat4& SceneNode::GetWorldTransform() const
    {
        return mWorldTransform;
    }


    const std::string& SceneNode::GetName() const
    {
        return mName;
    }

    SceneNodeID SceneNode::GetParentID() const
    {
        return mParent;
    }
}