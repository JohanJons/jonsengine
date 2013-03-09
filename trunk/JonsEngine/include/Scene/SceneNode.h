#pragma once

#include "include/Scene/Mesh.h"
#include "include/Core/Types.h"
#include "include/Core/Memory/HeapAllocator.h"

#include <string>
#include <vector>

namespace JonsEngine
{
    class IMemoryAllocator;

    /* SceneNode definition */
    class SceneNode
    {
    public:
        SceneNode(const std::string& nodeName);
        ~SceneNode();

        SceneNode* CreateChildNode(const std::string& nodeName);
        SceneNode* FindChildNode(const std::string& nodeName);
        bool DeleteChildNode(const std::string& nodeName);
        bool DeleteChildNode(SceneNode* node);

        void Scale(const Vec3& scaleVec);
        void Translate(const Vec3& translateVec);
        void Rotate(const float angle, const Vec3& rotateVec);

        void UpdateModelMatrix(const Mat4& parentModelMatrix);

        const Mat4& GetNodeTransform() const;
        const std::string& GetNodeName() const;
        const std::vector<SceneNode*>& GetChildNodes() const;

        bool operator==(const SceneNode& s1);
        bool operator==(const std::string& nodeName);


    private:
        // scene-specific data
        const std::string mName;
        size_t mHashedID;
        std::vector<SceneNode*> mChildNodes;
        IMemoryAllocator& mMemoryAllocator;

        // components
        Mat4 mModelMatrix;
        Quaternion mOrientation;
        Vec3 mScale;
        Vec3 mTranslation;
    };


    /* SceneNode inlines */
    inline const Mat4& SceneNode::GetNodeTransform() const                      { return mModelMatrix;  } 
    inline const std::string& SceneNode::GetNodeName() const                    { return mName;         }
    inline const std::vector<SceneNode*>& SceneNode::GetChildNodes() const      { return mChildNodes;   }
}