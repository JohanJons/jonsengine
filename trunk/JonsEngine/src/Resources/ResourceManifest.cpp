#include "include/Resources/ResourceManifest.h"
#include "include/Renderer/IRenderer.h"
#include "include/Renderer/Shapes.h"
#include "include/Core/Memory/HeapAllocator.h"

#include "boost/bind.hpp"
#include "boost/foreach.hpp"
#include "boost/assign.hpp"
#include "boost/algorithm/string.hpp"
#include <algorithm>
#include <sstream>

namespace JonsEngine
{
    ResourceManifest::ResourceManifest(IRenderer& renderer) : mRenderer(renderer), mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator())
    {
    }
       
    ResourceManifest::~ResourceManifest()
    {
    }


    ModelPtr ResourceManifest::CreateShape(const std::string& modelName, const ShapeType shapeType, const double sizeX, const double sizeY, const double sizeZ)
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

        ptr        = *mModels.insert(mModels.end(), ModelPtr(mMemoryAllocator.AllocateObject<Model>(modelName), boost::bind(&HeapAllocator::DeallocateObject<Model>, &mMemoryAllocator, _1)));
        ptr->mMesh = mRenderer.CreateMesh(vertexData, normalData, texcoordData, indiceData);

        return ptr;
    }

    
    ModelPtr ResourceManifest::LoadModel(const std::string& assetName, const JonsPackagePtr jonsPkg)
    {
        ModelPtr ptr = GetModel(assetName);
        if (ptr) 
            return ptr;

        std::vector<PackageModel>::iterator iter = std::find_if(jonsPkg->mModels.begin(), jonsPkg->mModels.end(), boost::bind(&boost::iequals<std::string, std::string>, boost::bind(&PackageModel::mName, _1), assetName, std::locale()));

        if (iter != jonsPkg->mModels.end())
        {
            ptr = *mModels.insert(mModels.end(), ModelPtr(mMemoryAllocator.AllocateObject<Model>(ProcessModel(*iter, jonsPkg)), boost::bind(&HeapAllocator::DeallocateObject<Model>, &mMemoryAllocator, _1)));
            mPackageAssetMap.insert(std::make_pair(jonsPkg->mInternalID , ptr->mName));
        }

        return ptr;
    }
        
    ModelPtr ResourceManifest::GetModel(const std::string& modelName)
    {
        std::vector<ModelPtr>::iterator iter = std::find_if(mModels.begin(), mModels.end(), boost::bind(&boost::iequals<std::string, std::string>, boost::bind(&Model::mName, _1), modelName, std::locale()));
        ModelPtr ptr;

        if (iter != mModels.end())
            ptr = *iter;

        return ptr;
    }


    MaterialPtr ResourceManifest::LoadMaterial(const std::string& assetName, const JonsPackagePtr jonsPkg)
    {
        MaterialPtr ptr = GetMaterial(assetName);
        if (ptr)
            return ptr;

        auto iter = std::find_if(jonsPkg->mMaterials.begin(), jonsPkg->mMaterials.end(), boost::bind(&boost::iequals<std::string, std::string>, boost::bind(&PackageMaterial::mName, _1), assetName, std::locale()));

        if (iter != jonsPkg->mMaterials.end())
        {
            ptr = *mMaterials.insert(mMaterials.end(), MaterialPtr(mMemoryAllocator.AllocateObject<Material>(ProcessMaterial(*iter, jonsPkg)), boost::bind(&HeapAllocator::DeallocateObject<Material>, &mMemoryAllocator, _1)));
            mPackageAssetMap.insert(std::make_pair(jonsPkg->mInternalID, ptr->mName));
        }

        return ptr;
    }
        
    MaterialPtr ResourceManifest::GetMaterial(const std::string& materialName)
    {
        std::vector<MaterialPtr>::iterator iter = std::find_if(mMaterials.begin(), mMaterials.end(), boost::bind(&boost::iequals<std::string, std::string>, boost::bind(&Material::mName, _1), materialName, std::locale()));
        MaterialPtr ptr;

        if (iter != mMaterials.end())
            ptr = *iter;

        return ptr;
    }


    Model ResourceManifest::ProcessModel(PackageModel& pkgModel, const JonsPackagePtr jonsPkg)
    {
        Model model(pkgModel.mName);
        model.mTransform = pkgModel.mTransform;
            
        BOOST_FOREACH(PackageMesh& mesh, pkgModel.mMeshes)
        {
            MaterialPtr material;

            if (mesh.mHasMaterial)
            {
                PackageMaterial& pkgMaterial = jonsPkg->mMaterials.at(mesh.mMaterialIndex);
                material = LoadMaterial(pkgMaterial.mName, jonsPkg);
            }

            model.mMesh     = mRenderer.CreateMesh(mesh.mVertexData, mesh.mNormalData, mesh.mTexCoordsData, mesh.mIndiceData);
            model.mMaterial = material;
        }

        BOOST_FOREACH(PackageModel& m, pkgModel.mChildren)
            model.mChildren.push_back(ProcessModel(m, jonsPkg));

        return model;
    }

    Material ResourceManifest::ProcessMaterial(PackageMaterial& pkgMaterial, const JonsPackagePtr jonsPkg)
    {
        TexturePtr diffuseTexture = mRenderer.CreateTexture(pkgMaterial.mDiffuseTexture.mTextureType, pkgMaterial.mDiffuseTexture.mTextureData, pkgMaterial.mDiffuseTexture.mTextureWidth,
                                                            pkgMaterial.mDiffuseTexture.mTextureHeight, pkgMaterial.mDiffuseTexture.mTextureFormat);

        return Material(pkgMaterial.mName, diffuseTexture, pkgMaterial.mDiffuseColor, pkgMaterial.mAmbientColor, pkgMaterial.mSpecularColor, pkgMaterial.mEmissiveColor);
    }

    bool ResourceManifest::ReloadAllResources()
    {
        // go through all asset files one at a time
        for(auto jonsPackagePair = mPackageAssetMap.begin(); jonsPackagePair != mPackageAssetMap.end(); jonsPackagePair = mPackageAssetMap.upper_bound(jonsPackagePair->first))
        {
            const uint32_t jonsPackageID = jonsPackagePair->first;
            const std::string& assetPath = GetJonsPkgPath(jonsPackageID);

            for (auto assets = mPackageAssetMap.begin(); assets != mPackageAssetMap.end(); [jonsPackageID] (std::pair<uint32_t, std::string> entry) { return entry.first == jonsPackageID; })
            {
                auto keke = GetModel(assets->second);
                Model* m = mMemoryAllocator.AllocateObject<Model>("asd");
                if (keke)
                    memcpy(keke.get(), m, sizeof(Model));

                assets++;
            }
        }

        return true;
    }
}