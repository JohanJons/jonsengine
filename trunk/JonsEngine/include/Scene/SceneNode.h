#pragma once

#include "include/Scene/Mesh.h"
#include "include/Core/Containers/Vector.h"
#include "include/Core/Types.h"

#include <string>

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

        void SetMesh();
        void SetMesh(MeshPtr mesh);
        
        const MeshPtr GetMesh() const;
        const Mat4& GetModelMatrix() const;
        const std::string& GetNodeName() const;
        const vector<SceneNode*>& GetChildNodes() const;

        bool operator==(const SceneNode& s1);
        bool operator==(const std::string& nodeName);


    private:
        // scene-specific data
        const std::string mName;
        size_t mHashedID;
        vector<SceneNode*> mChildNodes;
        IMemoryAllocator& mMemoryAllocator;

        // components
        MeshPtr mNodeMesh;
        Mat4 mModelMatrix;
        Quaternion mOrientation;
        Vec3 mScale;
        Vec3 mTranslation;
    };


    /* SceneNode inlines */
    inline const MeshPtr SceneNode::GetMesh() const                        { return mNodeMesh;     }
    inline const Mat4& SceneNode::GetModelMatrix() const                   { return mModelMatrix;  } 
    inline const std::string& SceneNode::GetNodeName() const               { return mName;         }
    inline const vector<SceneNode*>& SceneNode::GetChildNodes() const      { return mChildNodes;   }
}