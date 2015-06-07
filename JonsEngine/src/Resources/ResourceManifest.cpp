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
    ResourceManifest::ResourceManifest(DX11Renderer& renderer, IMemoryAllocatorPtr memoryAllocator) : mMemoryAllocator(memoryAllocator), mRenderer(renderer)
    {
    }
       
    ResourceManifest::~ResourceManifest()
    {
    }


    ModelPtr ResourceManifest::CreateRectangle(const std::string& modelName, const float sizeX, const float sizeY, const float sizeZ, MaterialPtr material)
    {
        ModelPtr model = GetModel(modelName);
        if (model)
            return model;

        assert(sizeX > 0 && sizeY > 0 && sizeZ > 0);

        std::vector<float> vertexData, normalData, texcoordData, tangents, bitangents;
        std::vector<uint16_t> indiceData;
        if (!CreateRectangleData(sizeX, sizeY, sizeZ, vertexData, normalData, texcoordData, indiceData))
            return model;

        const float halfX = sizeX / 2.0f;
        const float halfY = sizeY / 2.0f;
        const float halfZ = sizeZ / 2.0f;
        const Vec3 minBounds(-halfX, -halfY, -halfZ);
        const Vec3 maxBounds(halfX, halfY, halfZ);

		const MeshID meshID = mRenderer.CreateMesh(vertexData, normalData, texcoordData, tangents, bitangents, indiceData, minBounds, maxBounds);
		auto allocator = mMemoryAllocator;
        mModels.emplace_back(allocator->AllocateObject<Model>(modelName, Mat4(1.0f), minBounds, maxBounds, meshID, material, mTransformCache.GetStorage()), [=](Model* model) { allocator->DeallocateObject(model); });

        return mModels.back();
    }

    ModelPtr ResourceManifest::CreateCube(const std::string& modelName, const float size, MaterialPtr material)
    {
        return CreateRectangle(modelName, size, size, size, material);
    }

    ModelPtr ResourceManifest::CreateSphere(const std::string& modelName, const float radius, const uint32_t rings, const uint32_t sectors, MaterialPtr material)
    {
        ModelPtr model = GetModel(modelName);
        if (model)
            return model;

        assert(radius > 0 && rings > 0 && sectors > 0);

        std::vector<float> vertexData, normalData, texcoordData, tangents, bitangents;
        std::vector<uint16_t> indiceData;
        if (!CreateSphereData(radius, rings, sectors, vertexData, normalData, texcoordData, indiceData))
            return model;

        const Vec3 minBounds(-radius, -radius, -radius);
        const Vec3 maxBounds(radius, radius, radius);

		const MeshID meshID = mRenderer.CreateMesh(vertexData, normalData, texcoordData, tangents, bitangents, indiceData, minBounds, maxBounds);
		auto allocator = mMemoryAllocator;
        mModels.emplace_back(allocator->AllocateObject<Model>(modelName, Mat4(1.0f), minBounds, maxBounds, meshID, material, mTransformCache.GetStorage()), [=](Model* model) { allocator->DeallocateObject(model); });

		return mModels.back();
    }

    
    ModelPtr ResourceManifest::LoadModel(const std::string& assetName, const JonsPackagePtr jonsPkg)
    {
        ModelPtr model = GetModel(assetName);
        if (model)
            return model;

        size_t hashedName = boost::hash_value(assetName);
        std::vector<PackageModel>::iterator iter = std::find_if(jonsPkg->mModels.begin(), jonsPkg->mModels.end(), [hashedName](const PackageModel& pkgModel) { return boost::hash_value(pkgModel.mName) == hashedName; });

        if (iter != jonsPkg->mModels.end())
        {
            auto allocator = mMemoryAllocator;
			auto loadMaterialFunc = std::bind(&ResourceManifest::LoadMaterial, this, std::placeholders::_1, std::placeholders::_2);
            mModels.emplace_back(allocator->AllocateObject<Model>(mRenderer, jonsPkg, *iter, loadMaterialFunc, mTransformCache.GetStorage()), [=](Model* model) { allocator->DeallocateObject(model); });
            model = mModels.back();
        }

        return model;
    }
        
    ModelPtr ResourceManifest::GetModel(const std::string& modelName)
    {
        ModelPtr model;
        std::vector<ModelPtr>::iterator iter = std::find_if(mModels.begin(), mModels.end(), [modelName](const ModelPtr model) { return *model == modelName; });

        if (iter != mModels.end())
            model = *iter;

        return model;
    }


    MaterialPtr ResourceManifest::LoadMaterial(const std::string& assetName, const JonsPackagePtr jonsPkg)
    {
        MaterialPtr material = GetMaterial(assetName);
        if (material)
            return material;

        size_t hashedName = boost::hash_value(assetName);
        auto iter = std::find_if(jonsPkg->mMaterials.begin(), jonsPkg->mMaterials.end(), [hashedName](const PackageMaterial& pkgMaterial) { return boost::hash_value(pkgMaterial.mName) == hashedName; });

        if (iter != jonsPkg->mMaterials.end())
        {
			const PackageMaterial& pkgMaterial = *iter;

			TextureID diffuseTexture = INVALID_TEXTURE_ID;
			TextureID normalTexture  = INVALID_TEXTURE_ID;

			if (pkgMaterial.mHasDiffuseTexture)
				diffuseTexture = mRenderer.CreateTexture(TextureType::TEXTURE_TYPE_DIFFUSE, pkgMaterial.mDiffuseTexture.mTextureData, pkgMaterial.mDiffuseTexture.mTextureWidth, pkgMaterial.mDiffuseTexture.mTextureHeight);

			if (pkgMaterial.mHasNormalTexture)
                normalTexture = mRenderer.CreateTexture(TextureType::TEXTURE_TYPE_NORMAL, pkgMaterial.mNormalTexture.mTextureData, pkgMaterial.mNormalTexture.mTextureWidth, pkgMaterial.mNormalTexture.mTextureHeight);

			// TODO: real specular factor
			const float specularFactor = 0.02f;
            auto allocator = mMemoryAllocator;
			mMaterials.emplace_back(allocator->AllocateObject<Material>(pkgMaterial.mName, diffuseTexture, normalTexture, pkgMaterial.mDiffuseColor, pkgMaterial.mAmbientColor, pkgMaterial.mSpecularColor, pkgMaterial.mEmissiveColor, specularFactor), [=](Material* material) { allocator->DeallocateObject(material); });
            material = mMaterials.back();
        }

        return material;
    }
        
    MaterialPtr ResourceManifest::GetMaterial(const std::string& materialName)
    {
        MaterialPtr material;
        auto iter = std::find_if(mMaterials.begin(), mMaterials.end(), [materialName](const MaterialPtr material) { return *material == materialName; });

        if (iter != mMaterials.end())
            material = *iter;

        return material;
    }


    const std::vector<ModelPtr>& ResourceManifest::GetAllModels() const
    { 
        return mModels;
    }

    std::vector<ModelPtr>& ResourceManifest::GetAllModels()
    { 
        return mModels;
    }

    const IDMap<Mat4>& ResourceManifest::GetTransformStorage() const
    {
        return mTransformCache.GetStorage();
    }
}