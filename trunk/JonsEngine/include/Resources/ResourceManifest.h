#pragma once

#include "include/Resources/JonsPackage.h"
#include "include/Scene/Model.h"
#include "include/Scene/Material.h"
#include "include/Renderer/Shapes.h"

#include <vector>
#include <map>

namespace JonsEngine
{
    class IMemoryAllocator;
    class IRenderer;

    /* ResourceManifest definition */
    class ResourceManifest
    {
    public:
        enum ShapeType
        {
            UNKNOWN_SHAPE = 0,
            RECTANGLE
        };

        ResourceManifest(IRenderer& renderer);
        ~ResourceManifest();

        ModelPtr CreateShape(const std::string& modelName, const ShapeType shapeType, const double sizeX, const double sizeY, const double sizeZ);
        ModelPtr LoadModel(const std::string& assetName, const JonsPackagePtr jonsPkg);
        ModelPtr GetModel(const std::string& modelName);

        MaterialPtr LoadMaterial(const std::string& assetName, const JonsPackagePtr jonsPkg);
        MaterialPtr GetMaterial(const std::string& materialName);

        bool ReloadAllResources();

        const std::vector<ModelPtr>& GetAllModels() const;
        std::vector<ModelPtr>& GetAllModels();


    private:
        Model ProcessModel(PackageModel& pkgModel, const JonsPackagePtr jonsPkg);
        Material ProcessMaterial(PackageMaterial& pkgMaterial, const JonsPackagePtr jonsPkg);

        IRenderer& mRenderer;
        std::vector<ModelPtr> mModels;
        std::vector<MaterialPtr> mMaterials;
        std::multimap<uint32_t, std::string> mPackageAssetMap;
        IMemoryAllocator& mMemoryAllocator;
    };


    /* ResourceManifest inlines */
    inline const std::vector<ModelPtr>& ResourceManifest::GetAllModels() const      { return mModels; }
    inline std::vector<ModelPtr>& ResourceManifest::GetAllModels()                  { return mModels; }
}