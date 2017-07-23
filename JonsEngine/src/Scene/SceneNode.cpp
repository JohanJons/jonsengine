#include "include/Scene/SceneNode.h"
#include "include/Core/Math/Math.h"
#include "include/Core/Memory/HeapAllocator.h"

namespace JonsEngine
{
    SceneNode::SceneNode(const std::string& nodeName, const SceneNodeID parent, const OnSceneNodeDirtyFunc& onDirty) :
        mName(nodeName), mWorldTransform(gIdentityMatrix), mOrientation(1.0f, 0.0f, 0.0f, 0.0f), mScale(1.0f), mTranslation(0.0f), mParent(parent), mOnDirtyFunc(onDirty)
    {
    }

	SceneNode::SceneNode(const SceneNode& other) :
		mName(other.mName), mWorldTransform(other.mWorldTransform), mOrientation(other.mOrientation), mScale(other.mScale), mTranslation(other.mTranslation), mParent(other.mParent), mOnDirtyFunc(other.mOnDirtyFunc)
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
        mWorldTransform = glm::translate(gIdentityMatrix, mTranslation);
        mWorldTransform *= glm::toMat4(mOrientation);
        mWorldTransform = glm::scale(mWorldTransform, mScale);
    }

    void SceneNode::UpdateWorldMatrix(const Mat4& parentMatrix)
    {
        UpdateWorldMatrix();

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