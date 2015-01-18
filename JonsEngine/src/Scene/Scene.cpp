#include "include/Scene/Scene.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/EngineDefs.h"
#include "include/Core/Utils/Math.h"
#include "include/Resources/ResourceManifest.h"

#include "boost/functional/hash.hpp"
#include <algorithm>

namespace JonsEngine
{
    void AddMesh(std::vector<RenderableMesh>& resultMeshes, const Mesh& mesh, const Mat4& wvpMatrix, const Mat4& worldMatrix)
    {
        resultMeshes.emplace_back(mesh.mMeshID, wvpMatrix, worldMatrix);
    }

    void AddAllMeshes(std::vector<RenderableMesh>& resultMeshes, const ModelNode& node, const Mat4& wvpMatrix, const Mat4& worldMatrix)
    {
        const Mat4 localWVPMatrix = wvpMatrix * node.GetTransformMatrix();

        for (const Mesh& mesh : node.mMeshes)
            AddMesh(resultMeshes, mesh, localWVPMatrix, worldMatrix);

        for (const ModelNode& node : node.mChildNodes)
            AddAllMeshes(resultMeshes, node, wvpMatrix, worldMatrix);
    }

    void CullMeshes(std::vector<RenderableMesh>& resultMeshes, const ModelNode& node, const Mat4& wvpMatrix, const Mat4& worldMatrix)
    {
        const Mat4 localWVPMatrix = wvpMatrix * node.GetTransformMatrix();

        // test node frustrum
        FrustrumIntersection nodeAABBIntersection = IsAABBInFrustum(node.GetAABBCenter(), node.GetAABBExtent(), localWVPMatrix);
        switch (nodeAABBIntersection)
        {
            // if partially inside, recursively test all meshes and child nodes
        case FRUSTRUM_INTERSECTION_PARTIAL:
        {
            FrustrumIntersection meshAABBIntersection(FRUSTRUM_INTERSECTION_INSIDE);

            for (const Mesh& mesh : node.mMeshes)
            {
                meshAABBIntersection = IsAABBInFrustum(mesh.GetAABBCenter(), mesh.GetAABBExtent(), localWVPMatrix);
                if (meshAABBIntersection == FRUSTRUM_INTERSECTION_OUTSIDE)
                    continue;

                if (meshAABBIntersection == FRUSTRUM_INTERSECTION_INSIDE || meshAABBIntersection == FRUSTRUM_INTERSECTION_PARTIAL)
                    AddMesh(resultMeshes, mesh, wvpMatrix, worldMatrix);
            }

            for (const ModelNode& node : node.mChildNodes)
                CullMeshes(resultMeshes, node, wvpMatrix, worldMatrix);

            break;
        }

        case FRUSTRUM_INTERSECTION_INSIDE:
        {
            AddAllMeshes(resultMeshes, node, wvpMatrix, worldMatrix);
            break;
        }

        case FRUSTRUM_INTERSECTION_OUTSIDE:
        default:
            break;
        }
    }


    Scene::Scene(const std::string& sceneName) : mName(sceneName), mHashedID(boost::hash_value(sceneName)), mRootNode("Root"), mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator()), mAmbientLight(0.2f)
    {
    }

    Scene::~Scene()
    {
    }


    bool Scene::operator==(const Scene& s1)
    {
        return mHashedID == s1.mHashedID;
    }

    bool Scene::operator==(const std::string& sceneName)
    {
        return mHashedID == boost::hash_value(sceneName);
    }


    const RenderQueue& Scene::GetRenderQueue()
    {
        mRenderQueue.Clear();

        //CullMeshes(mRenderQueue.mCamera.mMeshes, mRootNode., )
        //mRenderQueue.

        return mRenderQueue;
    }


    Actor* Scene::CreateActor(const std::string& actorName, const ModelPtr model, const SceneNodePtr node)
    {
        mActors.emplace_back(mMemoryAllocator.AllocateObject<Actor>(actorName, model, node), std::bind(&HeapAllocator::DeallocateObject<Actor>, &mMemoryAllocator, std::placeholders::_1));

        return mActors.back().get();
    }

    void Scene::DeleteActor(Actor* actor)
    {
        auto iter = std::find_if(mActors.begin(), mActors.end(), [actor](const ActorPtr& storedActor) { return storedActor.get() == actor; });

        if (iter != mActors.end())
            mActors.erase(iter);
    }

    Actor* Scene::GetActor(const std::string& actorName)
    {
        size_t hashedID = boost::hash_value(actorName);

        auto iter = std::find_if(mActors.begin(), mActors.end(), [hashedID](const ActorPtr& storedActor) { return storedActor->mHashedID == hashedID; });
        if (iter == mActors.end())
            return nullptr;

        return iter->get();
    }


    PointLight* Scene::CreatePointLight(const std::string& lightName, const SceneNodePtr node)
    {
        mPointLights.emplace_back(mMemoryAllocator.AllocateObject<PointLight>(lightName, node), std::bind(&HeapAllocator::DeallocateObject<PointLight>, &mMemoryAllocator, std::placeholders::_1));

        return mPointLights.back().get();
    }
    
    void Scene::DeletePointLight(const PointLight* pointLight)
    {
        auto iter = std::find_if(mPointLights.begin(), mPointLights.end(), [pointLight](const PointLightPtr& storedLight) { return storedLight.get() == pointLight; });

        if (iter != mPointLights.end())
            mPointLights.erase(iter);
    }
    
    PointLight* Scene::GetPointLight(const std::string& lightName)
    {
        size_t hashedID = boost::hash_value(lightName);

        auto iter = std::find_if(mPointLights.begin(), mPointLights.end(), [hashedID](const PointLightPtr& storedLight) { return storedLight->mHashedID == hashedID; });
        if (iter == mPointLights.end())
            return nullptr;

        return iter->get();
    }
   
    const std::vector<PointLightPtr>& Scene::GetPointLights() const
    {
        return mPointLights;
    }
    

    DirectionalLight* Scene::CreateDirectionalLight(const std::string& lightName)
    {
        mDirectionalLights.emplace_back(mMemoryAllocator.AllocateObject<DirectionalLight>(lightName), std::bind(&HeapAllocator::DeallocateObject<DirectionalLight>, &mMemoryAllocator, std::placeholders::_1));

        return mDirectionalLights.back().get();
    }
    
    void Scene::DeleteDirectionalLight(const DirectionalLight* dirLight)
    {
        auto iter = std::find_if(mDirectionalLights.begin(), mDirectionalLights.end(), [dirLight](const DirectionalLightPtr& storedLight) { return storedLight.get() == dirLight; });

        if (iter != mDirectionalLights.end())
            mDirectionalLights.erase(iter);
    }
    
    DirectionalLight* Scene::GetDirectionalLight(const std::string& lightName)
    {
        size_t hashedID = boost::hash_value(lightName);

        auto iter = std::find_if(mDirectionalLights.begin(), mDirectionalLights.end(), [hashedID](const DirectionalLightPtr& storedLight) { return storedLight->mHashedID == hashedID; });
        if (iter == mDirectionalLights.end())
            return nullptr;

        return iter->get();
    }

    const std::vector<DirectionalLightPtr>& Scene::GetDirectionalLights() const
    {
        return mDirectionalLights;
    }
}