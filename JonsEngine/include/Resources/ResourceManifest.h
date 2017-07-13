#pragma once

#include "include/Resources/JonsPackage.h"
#include "include/Resources/Model.h"
#include "include/Resources/Animation.h"
#include "include/Resources/ModelNode.h"
#include "include/Resources/Mesh.h"
#include "include/Resources/Material.h"
#include "include/Resources/Skybox.h"
#include "include/Resources/TerrainData.h"
#include "include/Core/Containers/IDMap.hpp"
#include "include/Renderer/Shapes.h"

#include <unordered_map>
#include <string>

namespace JonsEngine
{
    class HeapAllocator;
    class DX11Renderer;

    class ResourceManifest
    {
    public:
        ResourceManifest(DX11Renderer& renderer, HeapAllocator& memoryAllocator);

        ModelID CreateRectangle(const std::string& modelName, float sizeX, float sizeY, float sizeZ);
        ModelID CreateCube(const std::string& modelName, float size);
        ModelID CreateSphere(const std::string& modelName, float radius, uint32_t rings, uint32_t sectors);
        ModelID LoadModel(const std::string& assetName, JonsPackagePtr jonsPkg);
        void DeleteModel(ModelID& modelID);
        const Model& GetModel(ModelID modelID) const;

		MaterialID CreateMaterial(const std::string& materialName, const std::string& diffuseTextureName, const std::string& normalTextureName, JonsPackagePtr jonsPkg);
        MaterialID LoadMaterial(const std::string& assetName, const JonsPackagePtr jonsPkg);
        void DeleteMaterial(MaterialID& materialID);
        const Material& GetMaterial(MaterialID materialID) const;

		TerrainDataID CreateTerrainData(const std::string& name, const std::string& heightmap, JonsPackagePtr jonsPkg);
		void DeleteTerrainData(TerrainDataID& terrainDataId);
		TerrainData& GetTerrainData(TerrainDataID terrainDataId);

        SkyboxID LoadSkybox(const std::string& skyboxName, const JonsPackagePtr jonsPkg);
        void DeleteSkybox(SkyboxID& skyboxID);
        Skybox& GetSkybox(SkyboxID skyboxID);
        const Skybox& GetSkybox(SkyboxID skyboxID) const;


    private:
        void ParseModelInitData(ModelNode::InitDataList& initData, JonsPackagePtr jongPkg, const PackageModel& model);
		DX11TextureID LoadTexture(const std::string& name, TextureType type, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight);
		bool HasCachedDXTexture(const std::string& name) const;


        HeapAllocator& mMemoryAllocator;
        DX11Renderer& mRenderer;

        IDMap<Model> mModels;
        IDMap<Material> mMaterials;
		IDMap<TerrainData> mTerrainData;
        IDMap<Skybox> mSkyboxes;

		std::unordered_map<std::string, DX11TextureID> mDXTextureMap;
    };
}