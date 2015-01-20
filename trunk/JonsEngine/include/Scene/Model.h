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

		bool operator==(const Model& m);
        bool operator==(const std::string& modelName);

        const Vec3& GetAABBCenter() const;
        const Vec3& GetAABBExtent() const;

		void SetSceneNode(SceneNodePtr node);
		SceneNodePtr GetSceneNode();

		const std::string& GetName() const;
		const Mat4& GetInitialTransform() const;
		std::vector<ModelNode>& GetModelNodes();

        
    private:
		const std::string mName;
        const size_t mHashedID;
        const Mat4 mInitialTransform;

        Vec3 mAABBCenter;
        Vec3 mAABBExtent;

		std::vector<ModelNode> mNodes;
        SceneNodePtr mSceneNode;
    };
}