#pragma once

#include "include/Core/Containers/Vector.h"
#include "include/Core/Utils/Types.h"

#include <string>

namespace JonsEngine
{
    class IMemoryAllocator;
    class Mesh;

    class SceneNode
    {
    public:
        SceneNode(const std::string& nodeName);
        ~SceneNode();

        SceneNode* CreateChildNode(const std::string& nodeName);
        SceneNode* FindChildNode(const std::string& nodeName);
        bool DeleteChildNode(const std::string& nodeName);
        bool DeleteChildNode(SceneNode* node);

        void Scale(Vec3 scaleVec);
        void Translate(Vec3 translateVec);
        void Rotate(Vec3 rotateVec, const float angle);

         

        const std::string& GetNodeName() const;
        const vector<SceneNode*>& GetChildNodes() const;

        bool operator==(SceneNode& s1);
        bool operator==(const std::string& nodeName);


    private:
        // scene-specific data
        const std::string mName;
        size_t mHashedID;
        Mat4 mTransform;
        vector<SceneNode*> mChildNodes;
        IMemoryAllocator& mMemoryAllocator;

        // components
        Mesh* mNodeMesh;
    };
}