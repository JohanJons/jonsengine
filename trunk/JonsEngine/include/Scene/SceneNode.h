#pragma once

#include "include/Core/Types.h"

#include <string>
#include <vector>
#include <memory>

namespace JonsEngine
{
    class IMemoryAllocator;
    class SceneNode;

    typedef std::shared_ptr<SceneNode> SceneNodePtr;

    class SceneNode
    {
    public:
        SceneNode(const std::string& nodeName);
        ~SceneNode();

        SceneNodePtr CreateChildNode(const std::string& nodeName);
        SceneNodePtr FindChildNode(const std::string& nodeName);
        bool RemoveChildNode(const std::string& nodeName);
        bool RemoveChildNode(SceneNodePtr node);

        void ScaleNode(const Vec3& scaleVec);
        void TranslateNode(const Vec3& translateVec);
        void RotateNode(const float angle, const Vec3& rotateVec);

        void UpdateModelMatrix(const Mat4& parentModelMatrix);

        Vec3 Position() const;

        const Mat4& GetNodeTransform() const;
        const std::string& GetNodeName() const;
        const std::vector<SceneNodePtr>& GetChildNodes() const;

        bool operator==(const SceneNode& s1);
        bool operator==(const std::string& nodeName);


    private:
        // scene-specific data
        const std::string mName;
        size_t mHashedID;
        std::vector<SceneNodePtr> mChildNodes;
        IMemoryAllocator& mMemoryAllocator;

        // base components
        Mat4 mModelMatrix;
        Quaternion mOrientation;
        Vec3 mScale;
        Vec3 mTranslation;
    };


    /* SceneNode inlines */
    inline const Mat4& SceneNode::GetNodeTransform() const                      { return mModelMatrix;  } 
    inline const std::string& SceneNode::GetNodeName() const                    { return mName;         }
    inline const std::vector<SceneNodePtr>& SceneNode::GetChildNodes() const    { return mChildNodes;   }
}