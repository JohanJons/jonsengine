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

        void PlayAnimationWuson();

        JonsEngine::Scene& GetJonsScene();
        JonsEngine::SceneNode& GetMovingLightNode();
        JonsEngine::DirectionalLight& GetSun();


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
        JonsEngine::SceneNodeID mNodeAnimWuson;
        JonsEngine::SceneNodeID mNodeWuson2;
        JonsEngine::SceneNodeID mNodePointLight;
        JonsEngine::SceneNodeID mNodePlane;
        JonsEngine::SceneNodeID mNodeSphere;
        JonsEngine::SceneNodeID mNodeCube2;
        JonsEngine::SceneNodeID mNodeCube3;

        JonsEngine::PointLightID mMovingPointLightID;
        JonsEngine::DirectionalLightID mSunDirLightID;

        JonsEngine::StaticActorID mActorSectoid;
        JonsEngine::StaticActorID mActorCube;
        JonsEngine::StaticActorID mActorChair;
        JonsEngine::StaticActorID mActorHouse;
        JonsEngine::AnimatedActorID mActorAnimWuson;
        JonsEngine::StaticActorID mActorWuson2;
        JonsEngine::StaticActorID mActorPlane;
        JonsEngine::StaticActorID mActorSphere;
        JonsEngine::StaticActorID mActorCube2;
        JonsEngine::StaticActorID mActorCube3;

        JonsEngine::ModelAnimationIndex mAnimIndexWuson;
        JonsEngine::Milliseconds mAnimDurationWuson;
    };
}