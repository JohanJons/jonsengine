#pragma once

#include "include/Scene/Scene.h"
#include "include/Core/Containers/IDMap.hpp"

#include <string>

namespace JonsEngine
{
    class IMemoryAllocator;
    class ResourceManifest;

    typedef IDMap<Scene>::ItemID SceneID;

    static const SceneID INVALID_SCENE_ID = IDMap<Scene>::INVALID_ITEM_ID;

    class SceneManager
    {
    public:
        SceneManager(const ResourceManifest& resourceManifest);
        ~SceneManager();

        SceneID CreateScene(const std::string& sceneName);
        Scene& GetScene(const SceneID sceneID);
        void DeleteScene(const SceneID scene);

		SceneID GetActiveScene() const;
        Scene& GetActiveScene();
        void SetActiveScene(const SceneID scene);


    private:
        const ResourceManifest& mResourceManifest;

        IDMap<Scene> mScenes;
        SceneID mActiveScene;
    };
}