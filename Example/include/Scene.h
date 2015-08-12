#pragma once

#include "include/Core/Engine.h"
#include "include/Resources/JonsPackage.h"
#include "include/Scene/Scene.h"

#include <string>

namespace JonsGame
{
    class Scene
    {
    public:
        Scene(JonsEngine::Scene& jonsScene, JonsEngine::ResourceManifest& resManifest, const std::string& jonsPkgName);
        ~Scene();

        JonsEngine::Scene& GetJonsScene();
        JonsEngine::PointLight& GetMovingPointLight();
        JonsEngine::DirectionalLight& GetSunDirLight();
        JonsEngine::Actor& GetAnimatedWuson();


    private:
        JonsEngine::Scene& mScene;
        JonsEngine::JonsPackagePtr mJonsPackage;

        JonsEngine::SkyboxID mSkybox;

        JonsEngine::MaterialID mMaterialCheckers;

        JonsEngine::ModelID mModelSectoid;
        JonsEngine::ModelID mModelCube;
        JonsEngine::ModelID mModelChair;
        JonsEngine::ModelID mModelHouse;
        JonsEngine::ModelID mModelWuson;
        JonsEngine::ModelID mModelPlane;
        JonsEngine::ModelID mModelSphere;
        JonsEngine::ModelID mModelCube2;
        JonsEngine::ModelID mModelCube3;

        JonsEngine::SceneNodePtr mNodeSectoid;
        JonsEngine::SceneNodePtr mNodeCube;
        JonsEngine::SceneNodePtr mNodeChair;
        JonsEngine::SceneNodePtr mNodeHouse;
        JonsEngine::SceneNodePtr mNodeWuson;
        JonsEngine::SceneNodePtr mNodePointLight;
        JonsEngine::SceneNodePtr mNodePlane;
        JonsEngine::SceneNodePtr mNodeSphere;
        JonsEngine::SceneNodePtr mNodeCube2;
        JonsEngine::SceneNodePtr mNodeCube3;

        JonsEngine::PointLightID mMovingPointLightID;
        JonsEngine::DirectionalLightID mSunDirLightID;

        JonsEngine::ActorID mActorSectoid;
        JonsEngine::ActorID mActorCube;
        JonsEngine::ActorID mActorChair;
        JonsEngine::ActorID mActorHouse;
        JonsEngine::ActorID mActorWuson;
        JonsEngine::ActorID mActorPlane;
        JonsEngine::ActorID mActorSphere;
        JonsEngine::ActorID mActorCube2;
        JonsEngine::ActorID mActorCube3;
    };
}