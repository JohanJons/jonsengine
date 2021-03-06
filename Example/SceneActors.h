#pragma once

#include "Core/Engine.h"
#include "Resources/JonsPackage.h"
#include "Scene/Scene.h"

#include "Sun.h"

#include <string>

namespace JonsGame
{
    class SceneActors
    {
    public:
        SceneActors( JonsEngine::SceneManager& sceneMgr, JonsEngine::ResourceManifest& resManifest );
        ~SceneActors();

		void Update();

		JonsEngine::SceneID GetSceneID() const { return mID; }

        void ToggleAnimationWuson();
		void ToggleAnimationBob();

        JonsEngine::Scene& GetJonsScene();
        JonsEngine::SceneNode& GetMovingLightNode();


    private:
		const JonsEngine::SceneNodeID mID;
        JonsEngine::SceneManager& mSceneMgr;
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
		JonsEngine::ModelID mModelBob;
		JonsEngine::ModelID mModelSAS;

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
		JonsEngine::SceneNodeID mNodeBob;
		JonsEngine::SceneNodeID mNodeSAS;

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
		JonsEngine::AnimatedActorID mActorAnimBob;
		JonsEngine::StaticActorID mActorSAS;

        JonsEngine::AnimationID mAnimWuson;
		JonsEngine::AnimationID mAnimBob;
        JonsEngine::Milliseconds mAnimDurationWuson;
		JonsEngine::Milliseconds mAnimDurationBob;
		bool mIsAnimatingWuson;
		bool mIsAnimatingBob;

		Sun mSun;
    };
}