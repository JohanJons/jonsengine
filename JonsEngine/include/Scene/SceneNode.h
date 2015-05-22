#pragma once

#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.h"

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace JonsEngine
{
    class IMemoryAllocator;
    class SceneNode;

    typedef std::function<void(SceneNode*)> OnSceneNodeDirtyFunc;
    typedef std::shared_ptr<SceneNode> SceneNodePtr;

    class SceneNode
    {
    public:
        SceneNode(const std::string& nodeName, const OnSceneNodeDirtyFunc& onDirty);
        ~SceneNode();
		
        bool operator==(const SceneNode& s1);
        bool operator==(const std::string& nodeName);

        SceneNodePtr CreateChildNode(const std::string& nodeName);
        SceneNodePtr FindChildNode(const std::string& nodeName);
        bool RemoveChildNode(const std::string& nodeName);
        bool RemoveChildNode(SceneNodePtr node);

        // TODO: for non-uniform scaling, need change in gbuffer shader
        void ScaleNode(const float scale);
        void TranslateNode(const Vec3& translateVec);
        void RotateNode(const float angle, const Vec3& rotateVec);

        void UpdateWorldMatrix();

        Vec3 Position() const;

        const Mat4& GetWorldMatrix() const;
        const std::string& GetNodeName() const;
        const std::vector<SceneNodePtr>& GetChildNodes() const;


    private:
        void UpdateTransform();
        void UpdateChildren(const Mat4& parentModelMatrix);


        // scene-specific data
        const std::string mName;
        size_t mHashedID;
        std::vector<SceneNodePtr> mChildNodes;
        IMemoryAllocator& mMemoryAllocator;

        // base components
        Mat4 mWorldMatrix;
        Quaternion mOrientation;
        Vec3 mScale;
        Vec3 mTranslation;

        const OnSceneNodeDirtyFunc mOnDirtyFunc;
    };


    /* SceneNode inlines */
    inline const Mat4& SceneNode::GetWorldMatrix() const                        { return mWorldMatrix;  }
    inline const std::string& SceneNode::GetNodeName() const                    { return mName;         }
    inline const std::vector<SceneNodePtr>& SceneNode::GetChildNodes() const    { return mChildNodes;   }
}