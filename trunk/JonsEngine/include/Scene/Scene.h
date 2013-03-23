#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Scene/SceneNode.h"
#include "include/Scene/Camera.h"
#include "include/Scene/Entity.h"
#include "include/Scene/Light.h"

#include <string>
#include <vector>

namespace JonsEngine
{
    class IMemoryAllocator;

    /* Scene definition */
    class Scene
    {
    public:
        typedef Vec4 LightColor;
        
        Scene(const std::string& sceneName);
        ~Scene();

        EntityPtr CreateEntity(const std::string& entityName);
        EntityPtr GetEntity(const std::string& entityName);
        const std::vector<EntityPtr>& GetAllEntities() const;
        void DeleteEntity(const std::string& entityName);

        LightPtr CreateLight(const std::string& lightName);
        LightPtr GetLight(const std::string& lightName);
        const std::vector<LightPtr>& GetAllLights() const;
        void DeleteLight(const std::string& lightName);

        void SetAmbientLight(const LightColor& ambientLight);
        const LightColor& GetAmbientLight() const;

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
        LightColor mAmbientLight;

        std::vector<EntityPtr> mEntities;
        std::vector<LightPtr>  mLights;
    };


    /* Scene inlines */
    inline Camera& Scene::GetSceneCamera()                     { return mSceneCamera;  }
    inline SceneNode& Scene::GetRootNode()                     { return mRootNode;     }
    inline const std::string& Scene::GetSceneName() const      { return mName;         }
}