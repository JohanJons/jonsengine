#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Scene/ModelNode.h"
#include "include/Scene/Material.h"
#include "include/Resources/JonsPackage.h"
#include "include/Core/Types.h"

#include <string>
#include <vector>
#include <memory>

namespace JonsEngine
{
    class Model
    {
    public:
        Model(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds);
        Model(const PackageModel& pkgModel);
        ~Model();

		bool operator==(const Model& m);
        bool operator==(const std::string& modelName);

		ModelNode& GetRootNode();

		const std::string& GetName() const;
        const size_t GetHashedName() const;

        
    private:
		const std::string mName;
        const size_t mHashedID;

		ModelNode mRootNode;
    };

	typedef std::shared_ptr<Model> ModelPtr;
}