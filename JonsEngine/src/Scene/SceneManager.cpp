#include "include/Scene/SceneManager.h"

namespace JonsEngine
{
    SceneManager::SceneManager(DX11Renderer& renderer, const ResourceManifest& resourceManifest) : mRenderer(renderer), mResourceManifest(resourceManifest), mActiveScene(CreateScene("DefaultScene"))
    {
    }

    SceneManager::~SceneManager()
    {
    }


    SceneID SceneManager::CreateScene(const std::string& sceneName)
    {
        return mScenes.Insert(sceneName, mRenderer, mResourceManifest);
    }

    Scene& SceneManager::GetScene(const SceneID sceneID)
    {
        return mScenes.GetItem(sceneID);
    }

    //void SceneManager::DeleteScene(SceneID& sceneID)
    //{
    //    mScenes.Erase(sceneID);
    //}


    SceneID SceneManager::GetActiveScene() const
    {
        return mActiveScene;
    }

    Scene& SceneManager::GetActiveScene()
    {
        return mScenes.GetItem(mActiveScene);
    }

    void SceneManager::SetActiveScene(const SceneID scene)
    {
        mActiveScene = scene;
    }
}