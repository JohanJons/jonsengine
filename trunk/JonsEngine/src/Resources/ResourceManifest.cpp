#include "include/Resources/ResourceManifest.h"
#include "include/Renderer/Shapes.h"
#include "include/Renderer/DirectX11/DX11Renderer.h"
#include "include/Core/Memory/HeapAllocator.h"

#include "boost/functional/hash.hpp"
#include <algorithm>
#include <sstream>

namespace JonsEngine
{
    ResourceManifest::ResourceManifest(DX11Renderer& renderer, IMemoryAllocatorPtr memoryAllocator) : mMemoryAllocator(memoryAllocator), mRenderer(renderer)
    {
    }
       
    ResourceManifest::~ResourceManifest()
    {
    }


    ModelPtr ResourceManifest::CreateRectangle(const std::string& modelName, const float sizeX, const float sizeY, const float sizeZ)
    {
        ModelPtr model = GetModel(modelName);
        if (model)
            return model;

        assert(sizeX > 0 && sizeY > 0 && sizeZ > 0);

        std::vector<float> vertexData, normalData, texcoordData, tangents, bitangents;
        std::vector<uint16_t> indiceData;
        if (!CreateRectangleData(sizeX, sizeY, sizeZ, vertexData, normalData, texcoordData, indiceData))
            return model;

        auto allocator = mMemoryAllocator;

        const float halfX = sizeX / 2.0f;
        const float halfY = sizeY / 2.0f;
        const float halfZ = sizeZ / 2.0f;
        Vec3 minBounds(-halfX, -halfY, -halfZ);
        Vec3 maxBounds(halfX, halfY, halfZ);

        model = *mModels.insert(mModels.end(), ModelPtr(allocator->AllocateObject<Model>(modelName, Mat4(1.0f), minBounds, maxBounds), [=](Model* model) { allocator->DeallocateObject(model); }));

        const MeshID meshID = mRenderer.CreateMesh(vertexData, normalData, texcoordData, tangents, bitangents, indiceData, minBounds, maxBounds);
        model->GetRootNode().GetMeshes().emplace_back("RectangleMesh", minBounds, maxBounds, meshID, nullptr);

        return model;
    }

    ModelPtr ResourceManifest::CreateCube(const std::string& modelName, const float size)
    {
        return CreateRectangle(modelName, size, size, size);
    }

    ModelPtr ResourceManifest::CreateSphere(const std::string& modelName, const float radius, const uint32_t rings, const uint32_t sectors)
    {
        ModelPtr model = GetModel(modelName);
        if (model)
            return model;

        assert(radius > 0 && rings > 0 && sectors > 0);

        std::vector<float> vertexData, normalData, texcoordData, tangents, bitangents;
        std::vector<uint16_t> indiceData;
        if (!CreateSphereData(radius, rings, sectors, vertexData, normalData, texcoordData, indiceData))
            return model;

        auto allocator = mMemoryAllocator;

        Vec3 minBounds(-radius, -radius, -radius);
        Vec3 maxBounds(radius, radius, radius);

        model = *mModels.insert(mModels.end(), ModelPtr(allocator->AllocateObject<Model>(modelName, Mat4(1.0f), minBounds, maxBounds), [=](Model* model) { allocator->DeallocateObject(model); }));
        
        const MeshID meshID = mRenderer.CreateMesh(vertexData, normalData, texcoordData, tangents, bitangents, indiceData, minBounds, maxBounds);
        model->GetRootNode().GetMeshes().emplace_back("SphereMesh", minBounds, maxBounds, meshID, nullptr);

        return model;
    }

    
    ModelPtr ResourceManifest::LoadModel(const std::string& assetName, const JonsPackagePtr jonsPkg)
    {
        ModelPtr model = GetModel(assetName);
        if (model)
            return model;

        size_t hashedName = boost::hash_value(assetName);
        std::vector<PackageModel>::iterator iter = std::find_if(jonsPkg->mModels.begin(), jonsPkg->mModels.end(), [hashedName](const PackageModel pkgModel) { return boost::hash_value(pkgModel.mName) == hashedName; });

        if (iter != jonsPkg->mModels.end())
        {
            auto allocator = mMemoryAllocator;
            mModels.emplace_back(allocator->AllocateObject<Model>(ProcessModel(*iter, jonsPkg)), [=](Model* model) { allocator->DeallocateObject(model); });
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
        auto iter = std::find_if(jonsPkg->mMaterials.begin(), jonsPkg->mMaterials.end(), [hashedName](const PackageMaterial pkgMaterial) { return boost::hash_value(pkgMaterial.mName) == hashedName; });

        if (iter != jonsPkg->mMaterials.end())
        {
            auto allocator = mMemoryAllocator;
            mMaterials.emplace_back(allocator->AllocateObject<Material>(ProcessMaterial(*iter, jonsPkg)), [=](Material* material) { allocator->DeallocateObject(material); });
            material = mMaterials.back();
        }

        return material;
    }
        
    MaterialPtr ResourceManifest::GetMaterial(const std::string& materialName)
    {
        MaterialPtr ptr;
        auto iter = std::find_if(mMaterials.begin(), mMaterials.end(), [materialName](const MaterialPtr material) { return *material == materialName; });

        if (iter != mMaterials.end())
            ptr = *iter;

        return ptr;
    }


    ModelPtr ResourceManifest::ProcessModel(const PackageModel& pkgModel, const JonsPackagePtr jonsPkg)
    {
        auto allocator = mMemoryAllocator;
        ModelPtr model(allocator->AllocateObject<Model>(pkgModel.mName, pkgModel.mRootNode.mTransform, pkgModel.mRootNode.mAABB.mMinBounds, pkgModel.mRootNode.mAABB.mMaxBounds), [=](Model* model) { allocator->DeallocateObject(model); });

        for (const PackageNode& node : pkgModel.mNodes)
            model->GetModelNodes().emplace_back(ProcessModelNode(node, jonsPkg));



      //      if (mesh.mHasMaterial)
      //      {
      //          PackageMaterial& pkgMaterial = jonsPkg->mMaterials.at(mesh.mMaterialIndex);
      //          model->mMaterial = LoadMaterial(pkgMaterial.mName, jonsPkg);
       //     }

       //     model->mMesh = mRenderer.CreateMesh(mesh.mVertexData, mesh.mNormalData, mesh.mTexCoordsData, mesh.mTangents, mesh.mBitangents, mesh.mIndiceData, mesh.mAABB.mMinBounds, mesh.mAABB.mMaxBounds);

     //   for(PackageModel& m : pkgModel.mChildren)
     //       model->mChildren.emplace_back(ProcessModel(m, jonsPkg));

        return model;
    }

    ModelNode ResourceManifest::ProcessModelNode(const PackageNode& pkgNode, const JonsPackagePtr jonsPkg)
    {
        ModelNode node(pkgNode.mName, pkgNode.mTransform, pkgNode.mAABB.mMinBounds, pkgNode.mAABB.mMaxBounds);

        for (const PackageMesh& mesh : pkgNode.mMeshes)
            node.mMeshes.emplace_back(ProcessMesh(mesh, jonsPkg));

        return node;
    }

    Mesh ResourceManifest::ProcessMesh(const PackageMesh& pkgMesh, const JonsPackagePtr jonsPkg)
    {
        const MeshID meshID = mRenderer.CreateMesh(pkgMesh.mVertexData, pkgMesh.mNormalData, pkgMesh.mTexCoordsData, pkgMesh.mTangents, pkgMesh.mBitangents, pkgMesh.mIndiceData,
            pkgMesh.mAABB.mMinBounds, pkgMesh.mAABB.mMaxBounds);

        MaterialPtr material(nullptr);
        if (pkgMesh.mHasMaterial)
        {
            PackageMaterial& pkgMaterial = jonsPkg->mMaterials.at(pkgMesh.mMaterialIndex);
            material = LoadMaterial(pkgMaterial.mName, jonsPkg);
        }

        return Mesh(pkgMesh.mName, pkgMesh.mAABB.mMinBounds, pkgMesh.mAABB.mMaxBounds, meshID, material);
    }

    Material ResourceManifest::ProcessMaterial(const PackageMaterial& pkgMaterial, const JonsPackagePtr jonsPkg)
    {
        TextureID diffuseTexture = INVALID_TEXTURE_ID;
        TextureID normalTexture  = INVALID_TEXTURE_ID;

        if (pkgMaterial.mHasDiffuseTexture)
            diffuseTexture = mRenderer.CreateTexture(pkgMaterial.mDiffuseTexture.mTextureType, pkgMaterial.mDiffuseTexture.mTextureData, pkgMaterial.mDiffuseTexture.mTextureWidth, pkgMaterial.mDiffuseTexture.mTextureHeight);

        if (pkgMaterial.mHasNormalTexture)
            normalTexture = mRenderer.CreateTexture(pkgMaterial.mNormalTexture.mTextureType, pkgMaterial.mNormalTexture.mTextureData, pkgMaterial.mNormalTexture.mTextureWidth, pkgMaterial.mNormalTexture.mTextureHeight);

        return Material(pkgMaterial.mName, diffuseTexture, normalTexture, pkgMaterial.mDiffuseColor, pkgMaterial.mAmbientColor, pkgMaterial.mSpecularColor, pkgMaterial.mEmissiveColor, 0.02f);
    }
}