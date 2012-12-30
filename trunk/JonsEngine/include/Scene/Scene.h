#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Scene/SceneNode.h"
#include "include/Scene/Camera.h"
#include "include/Core/Containers/Vector.h"

#include <string>

namespace JonsEngine
{
    class Renderer;

    /* Scene definition */
    class Scene
    {
    public:
        Scene(const std::string& sceneName);
        ~Scene();

        void Render(Renderer& renderer);
       
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
    };
}