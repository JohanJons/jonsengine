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

        ModelPtr LoadModel(const std::string& modelname, JonsPackagePtr jonsPkg);
        ModelPtr GetModel(const std::string& modelname);


    private:
        Model ProcessModel(PackageModel& pkgModel);

        IRenderer& mRenderer;
        std::vector<ModelPtr> mModels;
        IMemoryAllocator& mMemoryAllocator;
    };
}