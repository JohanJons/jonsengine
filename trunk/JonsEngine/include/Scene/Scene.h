#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Scene/SceneNode.h"
#include "include/Scene/Camera.h"
#include "include/Scene/Entity.h"

#include <string>
#include <vector>

namespace JonsEngine
{
    class IMemoryAllocator;

    /* Scene definition */
    class Scene
    {
    public:
        Scene(const std::string& sceneName);
        ~Scene();

        EntityPtr CreateEntity(const std::string& entityName);
        EntityPtr GetEntity(const std::string& entityName);
        const std::vector<EntityPtr>& GetAllEntities() const;
        void DeleteEntity(const std::string& entityName);

        Camera& GetSceneCamera();
        SceneNode& GetRootNode();
        const std::string& GetSceneName() const;

        bool operator==(const Scene& s1);
        bool operator==(const std::string& sceneName);


    private:
        const std::string mName;
        size_t mHashedID;
        Camera mSceneCamera;
        SceneNode mRootNode;
        IMemoryAllocator& mMemoryAllocator;

        std::vector<EntityPtr> mEntities;
    };


    /* Scene inlines */
    inline Camera& Scene::GetSceneCamera()                     { return mSceneCamera;  }
    inline SceneNode& Scene::GetRootNode()                     { return mRootNode;     }
    inline const std::string& Scene::GetSceneName() const      { return mName;         }
}