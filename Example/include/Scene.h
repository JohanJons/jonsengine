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
        JonsEngine::PointLight& GetMovingLight();
        JonsEngine::DirectionalLight& GetSun();
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

        JonsEngine::SceneNodeID mNodeSectoid;
        JonsEngine::SceneNodeID mNodeCube;
        JonsEngine::SceneNodeID mNodeChair;
        JonsEngine::SceneNodeID mNodeHouse;
        JonsEngine::SceneNodeID mNodeWuson;
        JonsEngine::SceneNodeID mNodeWuson2;
        JonsEngine::SceneNodeID mNodePointLight;
        JonsEngine::SceneNodeID mNodePlane;
        JonsEngine::SceneNodeID mNodeSphere;
        JonsEngine::SceneNodeID mNodeCube2;
        JonsEngine::SceneNodeID mNodeCube3;

        JonsEngine::PointLightID mMovingPointLightID;
        JonsEngine::DirectionalLightID mSunDirLightID;

        JonsEngine::ActorID mActorSectoid;
        JonsEngine::ActorID mActorCube;
        JonsEngine::ActorID mActorChair;
        JonsEngine::ActorID mActorHouse;
        JonsEngine::ActorID mActorWuson;
        JonsEngine::ActorID mActorWuson2;
        JonsEngine::ActorID mActorPlane;
        JonsEngine::ActorID mActorSphere;
        JonsEngine::ActorID mActorCube2;
        JonsEngine::ActorID mActorCube3;
    };
}