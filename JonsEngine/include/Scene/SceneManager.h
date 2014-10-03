#pragma once

#include <vector>

namespace JonsEngine
{
    class ResourceManifest;
    class IMemoryAllocator;
    class Scene;

    class SceneManager
    {
    public:
        SceneManager(ResourceManifest& resManifest);
        ~SceneManager();

        Scene* CreateScene(const std::string& sceneName);
        void DeleteScene(const std::string& sceneName);
        void DeleteScene(Scene* scene);
        Scene* FindScene(const std::string& sceneName);

        Scene* GetActiveScene() const;
        const std::vector<Scene*>& GetAllScenes() const;
        bool HasActiveScene() const;
        void SetActiveScene(const std::string& sceneName);


    private:
        std::vector<Scene*> mScenes;
        Scene* mActiveScene;

        ResourceManifest& mResourceManifest;
        IMemoryAllocator& mMemoryAllocator;
    };


    /* SceneManager inlines */
    inline Scene* SceneManager::GetActiveScene() const                      { return mActiveScene;            }  
    inline const std::vector<Scene*>& SceneManager::GetAllScenes() const    { return mScenes;                 }
    inline bool SceneManager::HasActiveScene() const                        { return mActiveScene != nullptr; }
}