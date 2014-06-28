#pragma once

#include "include/Resources/JonsPackage.h"
#include "include/Core/Memory/IMemoryAllocator.h"
#include "include/Scene/Model.h"
#include "include/Scene/Material.h"
#include "include/Renderer/DirectX11/DirectXRenderer.h"
#include "include/Renderer/Shapes.h"

#include <vector>
#include <map>

namespace JonsEngine
{
    class OpenGLRenderer;

    /* ResourceManifest definition */
    class ResourceManifest
    {
    public:
        ResourceManifest(DirectXRenderer& renderer, IMemoryAllocatorPtr memoryAllocator);
        ~ResourceManifest();

        ModelPtr CreateRectangle(const std::string& modelName, const double sizeX, const double sizeY, const double sizeZ);
        ModelPtr CreateCube(const std::string& modelName, const double size);
        ModelPtr CreateSphere(const std::string& modelName, const float radius, const uint32_t rings, const uint32_t sectors);
        ModelPtr LoadModel(const std::string& assetName, const JonsPackagePtr jonsPkg);
        ModelPtr GetModel(const std::string& modelName);

        MaterialPtr LoadMaterial(const std::string& assetName, const JonsPackagePtr jonsPkg);
        MaterialPtr GetMaterial(const std::string& materialName);

        const std::vector<ModelPtr>& GetAllModels() const;
        std::vector<ModelPtr>& GetAllModels();


    private:
        Model ProcessModel(PackageModel& pkgModel, const JonsPackagePtr jonsPkg);
        Material ProcessMaterial(PackageMaterial& pkgMaterial, const JonsPackagePtr jonsPkg);

        IMemoryAllocatorPtr mMemoryAllocator;
        DirectXRenderer& mRenderer;
        std::vector<ModelPtr> mModels;
        std::vector<MaterialPtr> mMaterials;
        std::multimap<uint32_t, std::string> mPackageAssetMap;
    };


    /* ResourceManifest inlines */
    inline const std::vector<ModelPtr>& ResourceManifest::GetAllModels() const      { return mModels; }
    inline std::vector<ModelPtr>& ResourceManifest::GetAllModels()                  { return mModels; }
}