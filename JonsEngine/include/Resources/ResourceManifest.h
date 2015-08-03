#pragma once

#include "include/Resources/JonsPackage.h"
#include "include/Resources/Model.h"
#include "include/Resources/ModelNode.h"
#include "include/Resources/Mesh.h"
#include "include/Resources/Material.h"
#include "include/Resources/Skybox.h"
#include "include/Core/Memory/IMemoryAllocator.h"
#include "include/Core/Containers/IDMap.hpp"
#include "include/Renderer/Shapes.h"

namespace JonsEngine
{
    class DX11Renderer;

    typedef IDMap<Model>::ItemID ModelID;
    typedef IDMap<Material>::ItemID MaterialID;
    typedef IDMap<Skybox>::ItemID SkyboxID;

    class ResourceManifest
    {
    public:
        ResourceManifest(DX11Renderer& renderer, IMemoryAllocatorPtr memoryAllocator);
        ~ResourceManifest();

        ModelID CreateRectangle(const std::string& modelName, const float sizeX, const float sizeY, const float sizeZ);
        ModelID CreateCube(const std::string& modelName, const float size);
        ModelID CreateSphere(const std::string& modelName, const float radius, const uint32_t rings, const uint32_t sectors);
        ModelID LoadModel(const std::string& assetName, const JonsPackagePtr jonsPkg);
        Model* GetModel(const ModelID modelID);

        MaterialID LoadMaterial(const std::string& assetName, const JonsPackagePtr jonsPkg);
        Material* GetMaterial(const MaterialID materialID);

        SkyboxID LoadSkybox(const std::string& skyboxName, const JonsPackagePtr jonsPkg);
        Skybox* GetSkybox(const SkyboxID skyboxID);

        const IDMap<Mat4>& GetTransformStorage() const;


    private:
        typedef IDMap<Model> ModelMap;
        typedef IDMap<Material> MaterialMap;
        typedef IDMap<Skybox> SkyboxMap;

        void ParseDX11Resources(DX11ResourceMeshTuples& meshResources, const JonsPackagePtr jongPkg, const PackageNode& node);


		IMemoryAllocatorPtr mMemoryAllocator;
        DX11Renderer& mRenderer;

        ModelMap mModels;
        MaterialMap mMaterials;
        SkyboxMap mSkyboxes;
    };
}