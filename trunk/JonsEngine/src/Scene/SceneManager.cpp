#include "include/Scene/SceneManager.h"
#include "include/Scene/Scene.h"
#include "include/Core/Memory/HeapAllocator.h"

#include "boost/foreach.hpp"
#include "boost/lambda/lambda.hpp"
#include <algorithm>

namespace JonsEngine
{
    SceneManager::SceneManager() : mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator()), mActiveScene(NULL)
    {
    }

    SceneManager::~SceneManager()
    {
        BOOST_FOREACH(Scene* scene, mScenes)
            mMemoryAllocator.DeallocateObject(scene);
    }

    Scene* SceneManager::CreateScene(const std::string& sceneName)
    {
        vector<Scene*>::iterator iter = std::find_if(mScenes.begin(), mScenes.end(), *boost::lambda::_1 == sceneName);
        
        if (iter == mScenes.end())
        {
            mScenes.push_back(mMemoryAllocator.AllocateObject<Scene>(sceneName));

            return mScenes.back();
        }
        else
            return NULL;
    }

    void SceneManager::DeleteScene(const std::string& sceneName)
    {
        vector<Scene*>::iterator iter = std::find_if(mScenes.begin(), mScenes.end(), *boost::lambda::_1 == sceneName);

        if (iter != mScenes.end())
        {
            if (mActiveScene && *mActiveScene == sceneName)
                mActiveScene = NULL;

            mMemoryAllocator.DeallocateObject(*iter);
            mScenes.erase(iter);
        }
    }

    void SceneManager::DeleteScene(Scene* scene)
    {
        vector<Scene*>::iterator iter = std::find_if(mScenes.begin(), mScenes.end(), boost::lambda::_1 == scene);

        if (iter != mScenes.end())
        {
            if (mActiveScene && *scene == *mActiveScene)
                mActiveScene = NULL;

            mMemoryAllocator.DeallocateObject(*iter);
            mScenes.erase(iter);
        }
    }
        
    Scene* SceneManager::FindScene(const std::string& sceneName)
    {
        vector<Scene*>::iterator iter = std::find_if(mScenes.begin(), mScenes.end(), *boost::lambda::_1 == sceneName);
        
        return iter != mScenes.end() ? *iter : NULL;
    }


    void SceneManager::SetActiveScene(const std::string& sceneName)
    {
        mActiveScene = FindScene(sceneName);
    }
}