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

        ModelPtr CreateRectangle(const double sizeX, const double sizeY, const double sizeZ);

        ModelPtr LoadModel(const std::string& modelName, const std::string& assetName, const JonsPackagePtr jonsPkg);
        ModelPtr GetModel(const std::string& modelName);

        MaterialPtr LoadMaterial(const std::string& materialName, const std::string& assetName, const JonsPackagePtr jonsPkg);
        MaterialPtr GetMaterial(const std::string& materialName);


    private:
        Model ProcessModel(PackageModel& pkgModel, const std::string& modelName, const JonsPackagePtr jonsPkg);
        Material ProcessMaterial(PackageMaterial& pkgMaterial, const std::string& materialName, const JonsPackagePtr jonsPkg);

        IRenderer& mRenderer;
        std::vector<ModelPtr> mImportedModels;
        std::vector<ModelPtr> mGeneratedModels;
        std::vector<MaterialPtr> mMaterials;
        IMemoryAllocator& mMemoryAllocator;
    };
}