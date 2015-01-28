#include "include/Scene/SceneManager.h"
#include "include/Scene/Scene.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/EngineSettings.h"

#include <algorithm>

namespace JonsEngine
{
    SceneManager::SceneManager() : mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator()), mActiveScene(CreateScene("DefaultScene"))
    {
    }

    SceneManager::~SceneManager()
    {
    }


    Scene* SceneManager::CreateScene(const std::string& sceneName)
    {
        auto iter = std::find_if(mScenes.begin(), mScenes.end(), [sceneName](const ScenePtr& scene) { return *scene == sceneName; });

        if (iter == mScenes.end())
        {
            mScenes.emplace_back(mMemoryAllocator.AllocateObject<Scene, const std::string&>(sceneName), std::bind(&HeapAllocator::DeallocateObject<Scene>, &mMemoryAllocator, std::placeholders::_1));

            return mScenes.back().get();
        }
        else
            return iter->get();
    }

    void SceneManager::DeleteScene(Scene* scene)
    {
        auto iter = std::find_if(mScenes.begin(), mScenes.end(), [scene](const ScenePtr& storedScene) { return storedScene.get() == scene; });

        if (iter != mScenes.end())
            mScenes.erase(iter);
    }

    const std::vector<ScenePtr>& SceneManager::GetAllScenes() const
    {
        return mScenes;
    }
}