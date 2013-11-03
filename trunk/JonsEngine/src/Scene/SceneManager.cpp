#include "include/Scene/SceneManager.h"
#include "include/Scene/Scene.h"
#include "include/Core/Memory/HeapAllocator.h"

#include <algorithm>

namespace JonsEngine
{
    SceneManager::SceneManager(ResourceManifest& resManifest) : mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator()), mActiveScene(nullptr), mResourceManifest(resManifest)
    {
    }

    SceneManager::~SceneManager()
    {
        for(Scene* scene : mScenes)
            mMemoryAllocator.DeallocateObject(scene);
    }

    Scene* SceneManager::CreateScene(const std::string& sceneName)
    {
        auto iter = std::find_if(mScenes.begin(), mScenes.end(), [sceneName](const Scene* scene) { return scene->GetSceneName() == sceneName; });
        
        if (iter == mScenes.end())
        {
            mScenes.push_back(mMemoryAllocator.AllocateObject<Scene, const std::string&, ResourceManifest&>(sceneName, mResourceManifest));

            return mScenes.back();
        }
        else
            return nullptr;
    }

    void SceneManager::DeleteScene(const std::string& sceneName)
    {
        auto iter = std::find_if(mScenes.begin(), mScenes.end(), [sceneName](const Scene* scene) { return scene->GetSceneName() == sceneName; });

        if (iter != mScenes.end())
        {
            if (mActiveScene && *mActiveScene == sceneName)
                mActiveScene = nullptr;

            mMemoryAllocator.DeallocateObject(*iter);
            mScenes.erase(iter);
        }
    }

    void SceneManager::DeleteScene(Scene* scene)
    {
        auto iter = std::find(mScenes.begin(), mScenes.end(), scene);

        if (iter != mScenes.end())
        {
            if (mActiveScene && *scene == *mActiveScene)
                mActiveScene = nullptr;

            mMemoryAllocator.DeallocateObject(*iter);
            mScenes.erase(iter);
        }
    }
        
    Scene* SceneManager::FindScene(const std::string& sceneName)
    {
        auto iter = std::find_if(mScenes.begin(), mScenes.end(), [sceneName](const Scene* scene) { return scene->GetSceneName() == sceneName; });
        
        return iter != mScenes.end() ? *iter : nullptr;
    }


    void SceneManager::SetActiveScene(const std::string& sceneName)
    {
        mActiveScene = FindScene(sceneName);
    }
}