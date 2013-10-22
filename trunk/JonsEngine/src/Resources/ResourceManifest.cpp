#include "include/Resources/ResourceManifest.h"
#include "include/Renderer/Shapes.h"
#include "include/Renderer/OpenGL3/OpenGLRenderer.h"
#include "include/Core/Memory/HeapAllocator.h"

#include "boost/functional/hash.hpp"
#include <algorithm>
#include <sstream>

namespace JonsEngine
{
    ResourceManifest::ResourceManifest(OpenGLRendererPtr renderer, IMemoryAllocatorPtr memoryAllocator) : mMemoryAllocator(memoryAllocator), mRenderer(renderer)
    {
    }
       
    ResourceManifest::~ResourceManifest()
    {
    }


    ModelPtr ResourceManifest::CreateRectangle(const std::string& modelName, const double sizeX, const double sizeY, const double sizeZ)
    {
        ModelPtr ptr = GetModel(modelName);
        if (ptr) 
            return ptr;

        if (sizeX <= 0 || sizeY <= 0 || sizeZ <= 0)
            return ptr;

        std::vector<float> vertexData;
        std::vector<float> normalData;
        std::vector<float> texcoordData;
        std::vector<uint32_t> indiceData;
        if (!CreateRectangleData(sizeX, sizeY, sizeZ, vertexData, normalData, texcoordData, indiceData))
            return ptr;

        auto allocator = mMemoryAllocator;
        ptr            = *mModels.insert(mModels.end(), ModelPtr(allocator->AllocateObject<Model>(modelName), [=](Model* model) { allocator->DeallocateObject(model); }));
        ptr->mMesh     = mRenderer->CreateMesh(vertexData, normalData, texcoordData, indiceData);

        return ptr;
    }

    
    ModelPtr ResourceManifest::LoadModel(const std::string& assetName, const JonsPackagePtr jonsPkg)
    {
        ModelPtr ptr = GetModel(assetName);
        if (ptr) 
            return ptr;

        size_t hashedName = boost::hash_value(assetName);
        std::vector<PackageModel>::iterator iter = std::find_if(jonsPkg->mModels.begin(), jonsPkg->mModels.end(), [hashedName](const PackageModel model) { return boost::hash_value(model.mName) == hashedName; });

        if (iter != jonsPkg->mModels.end())
        {
            auto allocator = mMemoryAllocator;
            ptr            = *mModels.insert(mModels.end(), ModelPtr(allocator->AllocateObject<Model>(ProcessModel(*iter, jonsPkg)), [=](Model* model) { allocator->DeallocateObject(model); }));
            mPackageAssetMap.insert(std::make_pair(jonsPkg->mInternalID , ptr->mName));
        }

        return ptr;
    }
        
    ModelPtr ResourceManifest::GetModel(const std::string& modelName)
    {
        ModelPtr ptr;
        size_t hashedName = boost::hash_value(modelName);
        std::vector<ModelPtr>::iterator iter = std::find_if(mModels.begin(), mModels.end(), [hashedName](const ModelPtr model) { return model->mHashedID == hashedName; });

        if (iter != mModels.end())
            ptr = *iter;

        return ptr;
    }


    MaterialPtr ResourceManifest::LoadMaterial(const std::string& assetName, const JonsPackagePtr jonsPkg)
    {
        MaterialPtr ptr = GetMaterial(assetName);
        if (ptr)
            return ptr;

        size_t hashedName = boost::hash_value(assetName);
        auto iter = std::find_if(jonsPkg->mMaterials.begin(), jonsPkg->mMaterials.end(), [hashedName](const PackageMaterial model) { return boost::hash_value(model.mName) == hashedName; });

        if (iter != jonsPkg->mMaterials.end())
        {
            auto allocator = mMemoryAllocator;
            ptr            = *mMaterials.insert(mMaterials.end(), MaterialPtr(allocator->AllocateObject<Material>(ProcessMaterial(*iter, jonsPkg)), [=](Material* material) { allocator->DeallocateObject(material); }));
            mPackageAssetMap.insert(std::make_pair(jonsPkg->mInternalID, ptr->mName));
        }

        return ptr;
    }
        
    MaterialPtr ResourceManifest::GetMaterial(const std::string& materialName)
    {
        MaterialPtr ptr;
        size_t hashedName = boost::hash_value(materialName);
        auto iter = std::find_if(mMaterials.begin(), mMaterials.end(), [hashedName](const MaterialPtr material) { return material->mHashedID == hashedName; });

        if (iter != mMaterials.end())
            ptr = *iter;

        return ptr;
    }


    Model ResourceManifest::ProcessModel(PackageModel& pkgModel, const JonsPackagePtr jonsPkg)
    {
        Model model(pkgModel.mName);
        model.mTransform = pkgModel.mTransform;
            
        for(PackageMesh& mesh : pkgModel.mMeshes)
        {
            if (mesh.mHasMaterial)
            {
                PackageMaterial& pkgMaterial = jonsPkg->mMaterials.at(mesh.mMaterialIndex);
                model.mMaterial = LoadMaterial(pkgMaterial.mName, jonsPkg);
            }

            model.mMesh = mRenderer->CreateMesh(mesh.mVertexData, mesh.mNormalData, mesh.mTexCoordsData, mesh.mIndiceData);
        }

        for(PackageModel& m : pkgModel.mChildren)
            model.mChildren.push_back(ProcessModel(m, jonsPkg));

        return model;
    }

    Material ResourceManifest::ProcessMaterial(PackageMaterial& pkgMaterial, const JonsPackagePtr jonsPkg)
    {
        TextureID diffuseTexture = mRenderer->CreateTexture(pkgMaterial.mDiffuseTexture.mTextureType, pkgMaterial.mDiffuseTexture.mTextureData, pkgMaterial.mDiffuseTexture.mTextureWidth,
                                                            pkgMaterial.mDiffuseTexture.mTextureHeight, pkgMaterial.mDiffuseTexture.mTextureFormat);

        return Material(pkgMaterial.mName, diffuseTexture, pkgMaterial.mDiffuseColor, pkgMaterial.mAmbientColor, pkgMaterial.mSpecularColor, pkgMaterial.mEmissiveColor, 0.02f);
    }
}