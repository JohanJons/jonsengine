#include "include/Resources/ResourceManifest.h"
#include "include/Renderer/Shapes.h"
#include "include/Renderer/DirectX11/DX11Renderer.h"
#include "include/Core/Memory/HeapAllocator.h"

#include "boost/functional/hash.hpp"
#include <algorithm>
#include <sstream>
#include <functional>
#include <tuple>
#include <utility>

namespace JonsEngine
{
    template <typename PackageStruct>
    typename std::vector<PackageStruct>::const_iterator FindInContainer(const std::string& assetName, const std::vector<PackageStruct>& container);
	typename std::vector<PackageTexture>::const_iterator FindTextureInContainer(const std::string& assetName, TextureType type, const std::vector<PackageTexture>& container);


    ResourceManifest::ResourceManifest(DX11Renderer& renderer, HeapAllocator& memoryAllocator) :
        mMemoryAllocator(memoryAllocator),
        mRenderer(renderer),
		mUnitCubeModelID( CreateCube( "_ENGINE_UNIT_CUBE", 1.0f ) )
    {
    }


    ModelID ResourceManifest::CreateRectangle(const std::string& modelName, const float sizeX, const float sizeY, const float sizeZ)
    {
        assert(sizeX > 0 && sizeY > 0 && sizeZ > 0);

        std::vector<float> vertexData, normalData, texcoordData, tangentData;
        std::vector<uint16_t> indiceData;
		CreateRectangleData(sizeX, sizeY, sizeZ, vertexData, normalData, texcoordData, indiceData);

        const float halfX = sizeX / 2.0f;
        const float halfY = sizeY / 2.0f;
        const float halfZ = sizeZ / 2.0f;
        const Vec3 minBounds(-halfX, -halfY, -halfZ);
        const Vec3 maxBounds(halfX, halfY, halfZ);

        const DX11MeshID meshID = mRenderer.CreateMesh(vertexData, normalData, texcoordData, tangentData, indiceData, minBounds, maxBounds);
        assert(meshID != INVALID_DX11_MESH_ID);

        return mModels.Insert(modelName, minBounds, maxBounds, meshID);
    }

    ModelID ResourceManifest::CreateCube(const std::string& modelName, const float size)
    {
        return CreateRectangle(modelName, size, size, size);
    }

    ModelID ResourceManifest::CreateSphere(const std::string& modelName, const float radius, const uint32_t rings, const uint32_t sectors)
    {
        assert(radius > 0 && rings > 0 && sectors > 0);

        std::vector<float> vertexData, normalData, texcoordData, tangentData;
        std::vector<uint16_t> indiceData;
		CreateSphereData(radius, rings, sectors, vertexData, normalData, texcoordData, indiceData);

        const Vec3 minBounds(-radius, -radius, -radius);
        const Vec3 maxBounds(radius, radius, radius);

        const DX11MeshID meshID = mRenderer.CreateMesh(vertexData, normalData, texcoordData, tangentData, indiceData, minBounds, maxBounds);
        assert(meshID != INVALID_DX11_MESH_ID);
        
        return mModels.Insert(modelName, minBounds, maxBounds, meshID);
    }

    ModelID ResourceManifest::LoadModel(const std::string& assetName, const JonsPackagePtr jonsPkg)
    {
        auto iter = FindInContainer<PackageModel>(assetName, jonsPkg->mModels);
        if (iter == jonsPkg->mModels.end())
            return INVALID_MODEL_ID;

        const PackageModel& pkgModel = *iter;

        ModelNode::InitDataList initDataList;
        ParseModelInitData(initDataList, jonsPkg, pkgModel);

        return mModels.Insert(pkgModel, initDataList);
    }

    void ResourceManifest::DeleteModel(ModelID& modelID)
    {
        assert(modelID != INVALID_MODEL_ID);
        const Model& model = GetModel(modelID);

        mModels.Erase(modelID);
        modelID = INVALID_MODEL_ID;
    }

    const Model& ResourceManifest::GetModel(const ModelID modelID) const
    {
        return mModels.GetItem(modelID);
    }

	const Model& ResourceManifest::GetUnitCubeModel() const
	{
		return GetModel( mUnitCubeModelID );
	}


	MaterialID ResourceManifest::CreateMaterial(const std::string& materialName, const std::string& diffuseTextureName, const std::string& normalTextureName, const JonsPackagePtr jonsPkg)
	{
		auto iter = FindInContainer<PackageMaterial>(materialName, jonsPkg->mMaterials);
		if (iter != jonsPkg->mMaterials.end())
			return INVALID_MATERIAL_ID;

		bool hasDiffuse = !diffuseTextureName.empty(), hasNormal = !normalTextureName.empty();

		auto endIter = jonsPkg->mTextures.cend();
		auto diffuseIter = endIter, normalIter = endIter, heightIter = endIter;
		if (hasDiffuse)
			diffuseIter = FindTextureInContainer(diffuseTextureName, TextureType::Diffuse, jonsPkg->mTextures);
		if (hasNormal)
			normalIter = FindTextureInContainer(normalTextureName, TextureType::Normal, jonsPkg->mTextures);

		bool allTexturesFound = (!hasDiffuse || diffuseIter != endIter) && (!hasNormal || normalIter != endIter);
		if (!allTexturesFound)
			return INVALID_MATERIAL_ID;

		DX11TextureID diffuseTexture = INVALID_DX11_TEXTURE_ID, normalTexture = INVALID_DX11_TEXTURE_ID, heightTexture = INVALID_DX11_TEXTURE_ID;
		if (hasDiffuse)
			diffuseTexture = LoadTexture(diffuseTextureName, TextureType::Diffuse, diffuseIter->mTextureData, diffuseIter->mTextureWidth, diffuseIter->mTextureHeight);
		if (hasNormal)
			normalTexture = LoadTexture(normalTextureName, TextureType::Normal, normalIter->mTextureData, normalIter->mTextureWidth, normalIter->mTextureHeight);

		// TODO: material colors
		return mMaterials.Insert(materialName, diffuseTexture, normalTexture);
	}

    MaterialID ResourceManifest::LoadMaterial(const std::string& assetName, const JonsPackagePtr jonsPkg)
    {
        auto iter = FindInContainer<PackageMaterial>(assetName, jonsPkg->mMaterials);
        if (iter == jonsPkg->mMaterials.end())
            return INVALID_MATERIAL_ID;
        const PackageMaterial& pkgMaterial = *iter;

        DX11TextureID diffuseTexture = INVALID_DX11_TEXTURE_ID, normalTexture  = INVALID_DX11_TEXTURE_ID;
		if (pkgMaterial.mDiffuseTexture != PackageTexture::INVALID_TEXTURE_INDEX)
		{
			auto& pkgTexture = jonsPkg->mTextures.at(pkgMaterial.mDiffuseTexture);
			diffuseTexture = LoadTexture(pkgTexture.mName, TextureType::Diffuse, pkgTexture.mTextureData, pkgTexture.mTextureWidth, pkgTexture.mTextureHeight);
		}
		if (pkgMaterial.mNormalTexture != PackageTexture::INVALID_TEXTURE_INDEX)
		{
			auto& pkgTexture = jonsPkg->mTextures.at(pkgMaterial.mNormalTexture);
			normalTexture = LoadTexture(pkgTexture.mName, TextureType::Normal, pkgTexture.mTextureData, pkgTexture.mTextureWidth, pkgTexture.mTextureHeight);
		}

        // TODO: material colors
        return mMaterials.Insert(pkgMaterial.mName, diffuseTexture, normalTexture, pkgMaterial.mDiffuseColor, pkgMaterial.mAmbientColor, pkgMaterial.mSpecularColor, pkgMaterial.mEmissiveColor, Material::DEFAULT_SPECULAR_FACTOR);
    }

    void ResourceManifest::DeleteMaterial(MaterialID& materialID)
    {
        assert(materialID != INVALID_MATERIAL_ID);

        mMaterials.Erase(materialID);
        materialID = INVALID_MATERIAL_ID;
    }

    const Material& ResourceManifest::GetMaterial(const MaterialID materialID) const
    {
        return mMaterials.GetItem(materialID);
    }


	TerrainDataID ResourceManifest::CreateTerrainData(const std::string& name, const std::string& heightmap, const JonsPackagePtr jonsPkg)
	{
		auto heightIter = FindTextureInContainer(heightmap, TextureType::Height, jonsPkg->mTextures);
		auto terrainMapIter = FindInContainer(heightmap, jonsPkg->mTerrainMaps);
		if (heightIter == jonsPkg->mTextures.end() || terrainMapIter == jonsPkg->mTerrainMaps.end())
			return INVALID_TERRAIN_DATA_ID;

		const float terrainWidth = heightIter->mTextureWidth;
		const float terrainHeight = heightIter->mTextureHeight;
		assert(terrainWidth > 0 && terrainHeight > 0);

		const auto textureData = heightIter->mTextureData;
		assert(!textureData.empty());

		const DX11TextureID heightmapTexture = LoadTexture(heightmap, TextureType::Height, textureData, terrainWidth, terrainHeight);

		const uint32_t texelDimensionPerPatch = 64;
		const uint32_t numVerticalPathes = ((terrainHeight - 1) / texelDimensionPerPatch) + 1;
		const uint32_t numHorizontalPathes = ((terrainWidth - 1) / texelDimensionPerPatch) + 1;

		const float minElevation = terrainMapIter->mMinElevation, maxElevation = terrainMapIter->mMaxElevation;

		return mTerrainData.Insert(name, minElevation, maxElevation, terrainWidth, terrainHeight, heightmapTexture);
	}

	void ResourceManifest::DeleteTerrainData(TerrainDataID& terrainDataId)
	{
		assert(terrainDataId != INVALID_TERRAIN_DATA_ID);

		mTerrainData.Erase(terrainDataId);
		terrainDataId = INVALID_TERRAIN_DATA_ID;
	}

	const TerrainData& ResourceManifest::GetTerrainData(const TerrainDataID terrainDataId) const
	{
		return mTerrainData.GetItem(terrainDataId);
	}


    SkyboxID ResourceManifest::LoadSkybox(const std::string& skyboxName, const JonsPackagePtr jonsPkg)
    {
        auto iter = FindTextureInContainer(skyboxName, TextureType::Skybox, jonsPkg->mTextures);
        if (iter == jonsPkg->mTextures.end())
            return INVALID_SKYBOX_ID;

        const PackageTexture& pkgSkyboxTexture = *iter;

        const DX11TextureID skyboxTextureID = LoadTexture(skyboxName, TextureType::Skybox, pkgSkyboxTexture.mTextureData, pkgSkyboxTexture.mTextureWidth, pkgSkyboxTexture.mTextureHeight);
        assert(skyboxTextureID != INVALID_DX11_TEXTURE_ID);

        return mSkyboxes.Insert(skyboxName, skyboxTextureID);
    }

    void ResourceManifest::DeleteSkybox(SkyboxID& skyboxID)
    {
        assert(skyboxID != INVALID_SKYBOX_ID);

        mSkyboxes.Erase(skyboxID);
        skyboxID = INVALID_SKYBOX_ID;
    }

    Skybox& ResourceManifest::GetSkybox(const SkyboxID skyboxID)
    {
        return mSkyboxes.GetItem(skyboxID);
    }

    const Skybox& ResourceManifest::GetSkybox(const SkyboxID skyboxID) const
    {
        return mSkyboxes.GetItem(skyboxID);
    }


    void ResourceManifest::ParseModelInitData(ModelNode::InitDataList& initDataList, const JonsPackagePtr jongPkg, const PackageModel& model)
    {
        for (const PackageNode& node : model.mNodes)
        {
            for (const PackageMesh::MeshIndex meshIndex : node.mMeshes)
            {
                const PackageMesh& mesh = model.mMeshes.at(meshIndex);

                const DX11MeshID meshID = mRenderer.CreateMesh(mesh.mVertexData, mesh.mNormalData, mesh.mTexCoordsData, mesh.mTangentData, mesh.mBoneWeights, mesh.mIndiceData, mesh.mAABB.mMinBounds, mesh.mAABB.mMaxBounds);
                DX11TextureID materialID = INVALID_DX11_TEXTURE_ID;
                const bool meshHasMaterial = mesh.mMaterialIndex != PackageMaterial::INVALID_MATERIAL_INDEX;
                if (meshHasMaterial)
                {
                    const PackageMaterial& material = jongPkg->mMaterials.at(mesh.mMaterialIndex);
                    materialID = LoadMaterial(material.mName, jongPkg);

                    assert(materialID != INVALID_DX11_TEXTURE_ID);
                }

                initDataList.emplace_back(mesh, meshID, materialID);
            }
        }
    }

	DX11TextureID ResourceManifest::LoadTexture(const std::string& name, TextureType type, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight)
	{
		bool isCached = HasCachedDXTexture(name);

		return isCached ? mDXTextureMap.at(name) : mRenderer.CreateTexture(type, textureData, textureWidth, textureHeight);
	}

	bool ResourceManifest::HasCachedDXTexture(const std::string& name) const
	{
		return mDXTextureMap.find(name) != mDXTextureMap.end();
	}


    template <typename PackageStruct>
    typename std::vector<PackageStruct>::const_iterator FindInContainer(const std::string& assetName, const std::vector<PackageStruct>& container)
    {
        const size_t hashedName = boost::hash_value(assetName);

        return std::find_if(container.begin(), container.end(), [hashedName](const PackageStruct& asset) { return boost::hash_value(asset.mName) == hashedName; });
    }

	typename std::vector<PackageTexture>::const_iterator FindTextureInContainer(const std::string& assetName, TextureType type, const std::vector<PackageTexture>& container)
	{
		const size_t hashedName = boost::hash_value(assetName);

		return std::find_if(container.begin(), container.end(), [hashedName, type](const PackageTexture& texture) { return boost::hash_value(texture.mName) == hashedName && texture.mType == type; });
	}
}