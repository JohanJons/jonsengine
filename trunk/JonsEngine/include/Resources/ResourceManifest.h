#pragma once

#include "include/Resources/JonsPackage.h"
#include "include/Scene/Model.h"
#include "include/Renderer/IRenderer.h"
#include "include/Core/Memory/HeapAllocator.h"

#include <vector>

namespace JonsEngine
{
    /* ResourceManifest definition */
    class ResourceManifest
    {
    public:
        ResourceManifest(IRenderer& renderer);
        ~ResourceManifest();

        ModelPtr LoadModel(const std::string& modelName, const std::string& assetName, JonsPackagePtr jonsPkg);
        ModelPtr GetModel(const std::string& modelName);


    private:
        Model ProcessModel(PackageModel& pkgModel, const std::string& modelName);

        IRenderer& mRenderer;
        std::vector<ModelPtr> mModels;
        IMemoryAllocator& mMemoryAllocator;
    };
}