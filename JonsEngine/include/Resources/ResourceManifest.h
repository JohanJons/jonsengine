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

    class ResourceManifest
    {
    public:
        ResourceManifest(DX11Renderer& renderer, IMemoryAllocatorPtr memoryAllocator);
        ~ResourceManifest();

        ModelID CreateRectangle(const std::string& modelName, const float sizeX, const float sizeY, const float sizeZ);
        ModelID CreateCube(const std::string& modelName, const float size);
        ModelID CreateSphere(const std::string& modelName, const float radius, const uint32_t rings, const uint32_t sectors);
        ModelID LoadModel(const std::string& assetName, const JonsPackagePtr jonsPkg);
        void DeleteModel(ModelID& modelID);
        Model& GetModel(const ModelID modelID);
        const Model& GetModel(const ModelID modelID) const;

        MaterialID LoadMaterial(const std::string& assetName, const JonsPackagePtr jonsPkg);
        void DeleteMaterial(MaterialID& materialID);
        Material& GetMaterial(const MaterialID materialID);
        const Material& GetMaterial(const MaterialID materialID) const;

        SkyboxID LoadSkybox(const std::string& skyboxName, const JonsPackagePtr jonsPkg);
        void DeleteSkybox(SkyboxID& skyboxID);
        Skybox& GetSkybox(const SkyboxID skyboxID);
        const Skybox& GetSkybox(const SkyboxID skyboxID) const;


    private:
        void ParseModelInitData(ModelNode::InitDataList& initData, const JonsPackagePtr jongPkg, const PackageNode& node);


		IMemoryAllocatorPtr mMemoryAllocator;
        DX11Renderer& mRenderer;

        IDMap<Model> mModels;
        IDMap<Material> mMaterials;
        IDMap<Skybox> mSkyboxes;
    };
}