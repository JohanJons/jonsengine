#pragma once

#include "include/Resources/JonsPackage.h"
#include "include/Scene/Model.h"
#include "include/Scene/Material.h"

#include <vector>

namespace JonsEngine
{
    class IMemoryAllocator;
    class IRenderer;

    /* ResourceManifest definition */
    class ResourceManifest
    {
    public:
        ResourceManifest(IRenderer& renderer);
        ~ResourceManifest();

        ModelPtr LoadModel(const std::string& modelName, const std::string& assetName, const JonsPackagePtr jonsPkg);
        ModelPtr GetModel(const std::string& modelName);
        bool HasModel(const std::string& modelName);

        MaterialPtr LoadMaterial(const std::string& materialName, const std::string& assetName, const JonsPackagePtr jonsPkg);
        MaterialPtr GetMaterial(const std::string& materialName);
        bool HasMaterial(const std::string& materialName);


    private:
        Model ProcessModel(PackageModel& pkgModel, const std::string& modelName, const JonsPackagePtr jonsPkg);
        Material ProcessMaterial(PackageMaterial& pkgMaterial, const std::string& materialName, const JonsPackagePtr jonsPkg);

        IRenderer& mRenderer;
        std::vector<ModelPtr> mModels;
        std::vector<MaterialPtr> mMaterials;
        IMemoryAllocator& mMemoryAllocator;
    };
}