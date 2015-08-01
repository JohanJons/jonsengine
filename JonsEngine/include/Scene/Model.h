#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Scene/ModelNode.h"
#include "include/Scene/Material.h"
#include "include/Scene/Animation.h"
#include "include/Resources/JonsPackage.h"
#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.hpp"

#include <string>
#include <vector>
#include <memory>

namespace JonsEngine
{
	class DX11Renderer;

    class Model
    {
    public:
        typedef std::vector<std::string> Animations;

        Model(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds, const MeshID meshID, MaterialPtr material, IDMap<Mat4>& transformStorage);
        Model(DX11Renderer& renderer, const JonsPackagePtr jonsPkg, const PackageModel& pkgModel, LoadMaterialFunc loadMaterialFunction, IDMap<Mat4>& transformStorage);
        ~Model();

		bool operator==(const Model& m);
        bool operator==(const std::string& modelName);

		ModelNode& GetRootNode();

        void PlayAnimation(const std::string animationName);


		const std::string mName;
        const Animations mAnimations;

        
    private:
        const size_t mHashedID;
		ModelNode mRootNode;
    };

	typedef std::shared_ptr<Model> ModelPtr;
}