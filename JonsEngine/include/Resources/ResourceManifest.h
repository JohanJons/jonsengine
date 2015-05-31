#pragma once

#include "include/Resources/JonsPackage.h"
#include "include/Core/Memory/IMemoryAllocator.h"
#include "include/Core/Containers/DataCache.hpp"
#include "include/Scene/Model.h"
#include "include/Scene/ModelNode.h"
#include "include/Scene/Mesh.h"
#include "include/Scene/Material.h"
#include "include/Renderer/DirectX11/DX11Renderer.h"
#include "include/Renderer/Shapes.h"

#include <vector>
#include <map>

namespace JonsEngine
{
    class OpenGLRenderer;

    class ResourceManifest
    {
    public:
        ResourceManifest(DX11Renderer& renderer, IMemoryAllocatorPtr memoryAllocator);
        ~ResourceManifest();

        ModelPtr CreateRectangle(const std::string& modelName, const float sizeX, const float sizeY, const float sizeZ, MaterialPtr material);
        ModelPtr CreateCube(const std::string& modelName, const float size, MaterialPtr material);
        ModelPtr CreateSphere(const std::string& modelName, const float radius, const uint32_t rings, const uint32_t sectors, MaterialPtr material);
        ModelPtr LoadModel(const std::string& assetName, const JonsPackagePtr jonsPkg);
        ModelPtr GetModel(const std::string& modelName);

        MaterialPtr LoadMaterial(const std::string& assetName, const JonsPackagePtr jonsPkg);
        MaterialPtr GetMaterial(const std::string& materialName);

        const std::vector<ModelPtr>& GetAllModels() const;
        std::vector<ModelPtr>& GetAllModels();


    private:
		IMemoryAllocatorPtr mMemoryAllocator;
        DataCache<Mat4> mTransformCache;
        DX11Renderer& mRenderer;

        std::vector<ModelPtr> mModels;
        std::vector<MaterialPtr> mMaterials;
    };


    /* ResourceManifest inlines */
    inline const std::vector<ModelPtr>& ResourceManifest::GetAllModels() const      { return mModels; }
    inline std::vector<ModelPtr>& ResourceManifest::GetAllModels()                  { return mModels; }
}