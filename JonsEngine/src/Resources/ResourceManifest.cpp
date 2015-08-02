#include "include/Resources/ResourceManifest.h"
#include "include/Renderer/Shapes.h"
#include "include/Renderer/DirectX11/DX11Renderer.h"
#include "include/Core/Memory/HeapAllocator.h"

#include "boost/functional/hash.hpp"
#include <algorithm>
#include <sstream>
#include <functional>

namespace JonsEngine
{
    template <typename PackageStruct>
    typename std::vector<PackageStruct>::const_iterator FindInContainer(const std::string& assetName, const std::vector<PackageStruct>& container);


    ResourceManifest::ResourceManifest(DX11Renderer& renderer, IMemoryAllocatorPtr memoryAllocator) : mMemoryAllocator(memoryAllocator), mRenderer(renderer)
    {
    }
       
    ResourceManifest::~ResourceManifest()
    {
    }


    ModelID ResourceManifest::CreateRectangle(const std::string& modelName, const float sizeX, const float sizeY, const float sizeZ)
    {
        assert(sizeX > 0 && sizeY > 0 && sizeZ > 0);

        std::vector<float> vertexData, normalData, texcoordData, tangentData;
        std::vector<uint16_t> indiceData;
        if (!CreateRectangleData(sizeX, sizeY, sizeZ, vertexData, normalData, texcoordData, indiceData))
            return ModelMap::INVALID_ITEM_ID;

        const float halfX = sizeX / 2.0f;
        const float halfY = sizeY / 2.0f;
        const float halfZ = sizeZ / 2.0f;
        const Vec3 minBounds(-halfX, -halfY, -halfZ);
        const Vec3 maxBounds(halfX, halfY, halfZ);

        const DX11MeshID meshID = mRenderer.CreateMesh(vertexData, normalData, texcoordData, tangentData, indiceData, minBounds, maxBounds);

        return mModels.AddItem(modelName, Mat4(1.0f), minBounds, maxBounds, meshID);
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
        if (!CreateSphereData(radius, rings, sectors, vertexData, normalData, texcoordData, indiceData))
            return ModelMap::INVALID_ITEM_ID;

        const Vec3 minBounds(-radius, -radius, -radius);
        const Vec3 maxBounds(radius, radius, radius);

        const DX11MeshID meshID = mRenderer.CreateMesh(vertexData, normalData, texcoordData, tangentData, indiceData, minBounds, maxBounds);
        
        return mModels.AddItem(modelName, Mat4(1.0f), minBounds, maxBounds, meshID);
    }

    ModelID ResourceManifest::LoadModel(const std::string& assetName, const JonsPackagePtr jonsPkg)
    {
        auto iter = FindInContainer<PackageModel>(assetName, jonsPkg->mModels);
        if (iter == jonsPkg->mModels.end())
            return ModelMap::INVALID_ITEM_ID;

        const PackageModel& pkgModel = *iter;

        auto loadMaterialFunc = std::bind(&ResourceManifest::LoadMaterial, this, std::placeholders::_1, std::placeholders::_2);

        return mModels.AddItem(mRenderer, jonsPkg, pkgModel, loadMaterialFunc);
    }

    Model* ResourceManifest::GetModel(const ModelID modelID)
    {
        return mModels.TryGetItem(modelID);
    }


    MaterialID ResourceManifest::LoadMaterial(const std::string& assetName, const JonsPackagePtr jonsPkg)
    {
        auto iter = FindInContainer<PackageMaterial>(assetName, jonsPkg->mMaterials);
        if (iter == jonsPkg->mMaterials.end())
            return MaterialMap::INVALID_ITEM_ID;

        const PackageMaterial& pkgMaterial = *iter;

        DX11MaterialID diffuseTexture = INVALID_TEXTURE_ID;
        DX11MaterialID normalTexture  = INVALID_TEXTURE_ID;

        if (pkgMaterial.mHasDiffuseTexture)
        	diffuseTexture = mRenderer.CreateTexture(TextureType::TEXTURE_TYPE_DIFFUSE, pkgMaterial.mDiffuseTexture.mTextureData, pkgMaterial.mDiffuseTexture.mTextureWidth, pkgMaterial.mDiffuseTexture.mTextureHeight);

        if (pkgMaterial.mHasNormalTexture)
            normalTexture = mRenderer.CreateTexture(TextureType::TEXTURE_TYPE_NORMAL, pkgMaterial.mNormalTexture.mTextureData, pkgMaterial.mNormalTexture.mTextureWidth, pkgMaterial.mNormalTexture.mTextureHeight);

        // TODO: real specular factor
        const float specularFactor = 0.02f;

        // TODO: material colors
        return mMaterials.AddItem(pkgMaterial.mName, diffuseTexture, normalTexture, pkgMaterial.mDiffuseColor, pkgMaterial.mAmbientColor, pkgMaterial.mSpecularColor, pkgMaterial.mEmissiveColor, specularFactor);
    }

    Material* ResourceManifest::GetMaterial(const MaterialID materialID)
    {
        return mMaterials.TryGetItem(materialID);
    }


    SkyboxID ResourceManifest::LoadSkybox(const std::string& skyboxName, const JonsPackagePtr jonsPkg)
    {
        auto iter = FindInContainer<PackageSkybox>(skyboxName, jonsPkg->mSkyBoxes);
        if (iter == jonsPkg->mSkyBoxes.end())
            return SkyboxMap::INVALID_ITEM_ID;

        const PackageSkybox& pkgSkybox = *iter;
        const PackageTexture& pkgSkyboxTexture = pkgSkybox.mSkyboxTexture;

        const DX11MaterialID skyboxTextureID = mRenderer.CreateTexture(TextureType::TEXTURE_TYPE_SKYBOX, pkgSkyboxTexture.mTextureData, pkgSkyboxTexture.mTextureWidth, pkgSkyboxTexture.mTextureHeight);

        return mSkyboxes.AddItem(skyboxName, skyboxTextureID);

    }

    Skybox* ResourceManifest::GetSkybox(const SkyboxID skyboxID)
    {
        return mSkyboxes.TryGetItem(skyboxID);
    }


    template <typename PackageStruct>
    typename std::vector<PackageStruct>::const_iterator FindInContainer(const std::string& assetName, const std::vector<PackageStruct>& container)
    {
        size_t hashedName = boost::hash_value(assetName);

        return std::find_if(container.begin(), container.end(), [hashedName](const PackageStruct& asset) { return boost::hash_value(asset.mName) == hashedName; });
    }
}