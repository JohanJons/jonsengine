#pragma once

#include "include/Core/Containers/Vector.h"

namespace JonsEngine
{
    class Scene;
    class IMemoryAllocator;

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
        void SetActiveScene(const std::string& sceneName);


    private:
        vector<Scene*> mScenes;
        Scene* mActiveScene;
        IMemoryAllocator& mMemoryAllocator;
    };
}