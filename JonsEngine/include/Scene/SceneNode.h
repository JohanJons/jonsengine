#pragma once

#include "include/Core/Types.h"
#include "include/Core/Containers/IDMapTree.hpp"

#include <string>
#include <vector>
#include <functional>

namespace JonsEngine
{
    class SceneNode;

    typedef std::function<void(SceneNode*)> OnSceneNodeDirtyFunc;
    typedef IDMapTree<SceneNode>::ItemID SceneNodeID;
    static const SceneNodeID INVALID_SCENE_NODE_ID = IDMapTree<SceneNode>::INVALID_ITEM_ID;

    class SceneNode
    {
    public:
        SceneNode(const std::string& nodeName, const SceneNodeID parent, const OnSceneNodeDirtyFunc& onDirty);
		SceneNode(const SceneNode& otherSceneNode);
        //SceneNode& operator=(SceneNode&& otherSceneNode);
        ~SceneNode();
	
        // TODO: for non-uniform scaling, need change in gbuffer shader
        void ScaleNode(const float scale);
        void TranslateNode(const Vec3& translateVec);
        void RotateNode(const float angle, const Vec3& rotateVec);

        void UpdateWorldMatrix();
        void UpdateWorldMatrix(const Mat4& parentMatrix);

        const Vec3& Position() const;
        const Vec3& Scale() const;
        const Mat4& GetWorldTransform() const;

        const std::string& GetName() const;
        SceneNodeID GetParentID() const;


    private:
        std::string mName;
        Mat4 mWorldTransform;
        Quaternion mOrientation;
        Vec3 mScale;
        Vec3 mTranslation;

        SceneNodeID mParent;
        OnSceneNodeDirtyFunc mOnDirtyFunc;
    };
}