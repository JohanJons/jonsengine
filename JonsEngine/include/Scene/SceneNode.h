#pragma once

#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.hpp"

#include <string>
#include <vector>
#include <functional>

namespace JonsEngine
{
    class SceneNode;

    typedef std::function<void(SceneNode*)> OnSceneNodeDirtyFunc;
    typedef IDMap<SceneNode>::ItemID SceneNodeID;
    static const SceneNodeID INVALID_SCENE_NODE_ID = IDMap<SceneNode>::INVALID_ITEM_ID;

    class SceneNode
    {
    public:
        SceneNode(const std::string& nodeName, const SceneNodeID parent, const OnSceneNodeDirtyFunc& onDirty);
        ~SceneNode();
	
        /*SceneNodeID CreateChildNode(const std::string& nodeName);
        SceneNodeID FindChildNode(const std::string& nodeName);
        bool RemoveChildNode(const std::string& nodeName);
        bool RemoveChildNode(SceneNodePtr node);*/

        // TODO: for non-uniform scaling, need change in gbuffer shader
        void ScaleNode(const float scale);
        void TranslateNode(const Vec3& translateVec);
        void RotateNode(const float angle, const Vec3& rotateVec);

        void UpdateWorldMatrix(const Mat4& parentMatrix);

        const Vec3& Position() const;
        const Vec3& Scale() const;
        const Mat4& GetWorldTransform() const;


        const std::string mName;
        const SceneNodeID mParent;
        //IDMap<Mat4>::ItemID GetWorldTransformID() const;
        //const std::string& GetNodeName() const;
        //const std::vector<SceneNodePtr>& GetChildNodes() const;


    private:
        Mat4 mWorldTransform;
        Quaternion mOrientation;
        Vec3 mScale;
        Vec3 mTranslation;

        const OnSceneNodeDirtyFunc mOnDirtyFunc;
    };
}