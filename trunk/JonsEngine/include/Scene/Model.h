#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Scene/ModelNode.h"
#include "include/Scene/Material.h"
#include "include/Core/Types.h"

#include <string>
#include <vector>
#include <memory>

namespace JonsEngine
{
    class Model;

    typedef std::shared_ptr<Model> ModelPtr;

    class Model
    {
    public:
        Model(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds);
        ~Model();

        const Vec3& GetAABBCenter() const;
        const Vec3& GetAABBExtent() const;

        bool operator==(const Model& m);
        bool operator==(const std::string& modelName);


        const std::string mName;
        const size_t mHashedID;

        std::vector<ModelNode> mNodes;
        SceneNodePtr mSceneNode;

        
    private:
        Vec3 mAABBCenter;
        Vec3 mAABBExtent;
    };
}