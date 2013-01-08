#pragma once

#include "include/Core/Containers/Vector.h"

namespace JonsEngine
{
    class Scene;
    class IMemoryAllocator;

    /* SceneManager definition */
    class SceneManager
    {
    public:
        SceneManager();
        ~SceneManager();

        Scene* CreateScene(const std::string& sceneName);
        void DeleteScene(const std::string& sceneName);
        void DeleteScene(Scene* scene);
        Scene* FindScene(const std::string& sceneName);

        Scene* GetActiveScene() const;
        const vector<Scene*>& GetAllScenes() const;
        bool HasActiveScene() const;
        void SetActiveScene(const std::string& sceneName);


    private:
        vector<Scene*> mScenes;
        Scene* mActiveScene;
        IMemoryAllocator& mMemoryAllocator;
    };


    /* SceneManager inlines */
    inline Scene* SceneManager::GetActiveScene() const                 { return mActiveScene;          }  
    inline const vector<Scene*>& SceneManager::GetAllScenes() const    { return mScenes;               }
    inline bool SceneManager::HasActiveScene() const                   { return mActiveScene != NULL;  }
}