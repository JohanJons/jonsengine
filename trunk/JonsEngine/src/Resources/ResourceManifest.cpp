#include "include/Resources/ResourceManifest.h"
#include "include/Scene/Model.h"

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

    
    ModelPtr ResourceManifest::LoadModel(const std::string& modelname, JonsPackagePtr jonsPkg)
    {
        std::vector<PackageModel>::iterator iter = std::find_if(jonsPkg->mModels.begin(), jonsPkg->mModels.end(), boost::bind(&PackageModel::mName, _1) == modelname);
        ModelPtr ptr;

        if (iter != jonsPkg->mModels.end())
        {
            ptr = ModelPtr(mMemoryAllocator.AllocateObject<Model>(ProcessModel(*iter)),
                           boost::bind(&HeapAllocator::DeallocateObject<Model>, &mMemoryAllocator, _1));

            mModels.push_back(ptr);
        }

        return ptr;
    }
        
    ModelPtr ResourceManifest::GetModel(const std::string& modelname)
    {
        std::vector<ModelPtr>::iterator iter = std::find_if(mModels.begin(), mModels.end(), boost::bind(&Model::mName, _1) == modelname);
        ModelPtr ptr;

        if (iter != mModels.end())
            ptr = *iter;

        return ptr;
    }

    Model ResourceManifest::ProcessModel(PackageModel& pkgModel)
    {
        Model model(pkgModel.mName);
        model.mTransform = pkgModel.mTransform;
            
        BOOST_FOREACH(PackageMesh& mesh, pkgModel.mMeshes)
            model.mMeshes.push_back(Mesh(mRenderer.CreateVertexBuffer(mesh.mVertexData, mesh.mIndiceData)));

        BOOST_FOREACH(PackageModel& m, pkgModel.mChildren)
            model.mChildren.push_back(ProcessModel(m));

        return model;
    }
}