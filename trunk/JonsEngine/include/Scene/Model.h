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
	class DX11Renderer;

    class Model
    {
    public:
		Model(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds, const MeshID meshID, MaterialPtr material);
        Model(DX11Renderer& renderer, const JonsPackagePtr jonsPkg, const PackageModel& pkgModel, LoadMaterialFunc loadMaterialFunction);
        ~Model();

		bool operator==(const Model& m);
        bool operator==(const std::string& modelName);

        float GetTextureTilingFactor() const;
        void SetTextureTilingFactor(const float tilingFactor);


		const ModelNode mRootNode;
		const std::string mName;

        
    private:
        const size_t mHashedID;
        float mTextureTilingFactor;
    };

	typedef std::shared_ptr<Model> ModelPtr;
}