#include "include/Resources/ResourceManifest.h"
#include "include/Scene/Model.h"
#include "include/Renderer/IRenderer.h"
#include "include/Core/Memory/HeapAllocator.h"

#include "boost/bind.hpp"
#include "boost/foreach.hpp"
#include <algorithm>

namespace JonsEngine
{
    ResourceManifest::ResourceManifest(IRenderer& renderer) : mRenderer(renderer), mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator())
    {
    }
       
    ResourceManifest::~ResourceManifest()
    {
    }

    
    ModelPtr ResourceManifest::LoadModel(const std::string& modelName, const std::string& assetName, JonsPackagePtr jonsPkg)
    {
        std::vector<PackageModel>::iterator iter = std::find_if(jonsPkg->mModels.begin(), jonsPkg->mModels.end(), boost::bind(&PackageModel::mName, _1) == assetName);
        ModelPtr ptr;

        if (iter != jonsPkg->mModels.end())
        {
            ptr = ModelPtr(mMemoryAllocator.AllocateObject<Model>(ProcessModel(*iter, modelName)),
                           boost::bind(&HeapAllocator::DeallocateObject<Model>, &mMemoryAllocator, _1));

            mModels.push_back(ptr);
        }

        return ptr;
    }
        
    ModelPtr ResourceManifest::GetModel(const std::string& modelName)
    {
        std::vector<ModelPtr>::iterator iter = std::find_if(mModels.begin(), mModels.end(), boost::bind(&Model::mName, _1) == modelName);
        ModelPtr ptr;

        if (iter != mModels.end())
            ptr = *iter;

        return ptr;
    }

    Model ResourceManifest::ProcessModel(PackageModel& pkgModel, const std::string& modelName)
    {
        Model model(modelName);
        model.mTransform = pkgModel.mTransform;
            
        BOOST_FOREACH(PackageMesh& mesh, pkgModel.mMeshes)
            model.mMeshes.push_back(Mesh(mRenderer.CreateVertexBuffer(mesh.mVertexData, mesh.mNormalData, mesh.mIndiceData)));

        BOOST_FOREACH(PackageModel& m, pkgModel.mChildren)
            model.mChildren.push_back(ProcessModel(m, m.mName));

        return model;
    }
}