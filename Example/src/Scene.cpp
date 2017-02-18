#include "include/Scene.h"

#include "include/Core/Types.h"

using namespace JonsEngine;

namespace JonsGame
{
    Scene::Scene(JonsEngine::Scene& jonsScene, JonsEngine::ResourceManifest& resManifest, const std::string& jonsPkgName) :
        // base
        mScene(jonsScene),
        mJonsPackage(ReadJonsPkg("assets.jons")),

        // skybox
        mSkybox(resManifest.LoadSkybox("skybox", mJonsPackage)),

        // materials
        mMaterialCheckers(resManifest.CreateMaterial("checkers", "checkers", ResourceManifest::NO_TEXTURE, ResourceManifest::NO_TEXTURE, mJonsPackage)),
		mMaterialTerrain(resManifest.CreateMaterial("terrain", "checkers", ResourceManifest::NO_TEXTURE, "heightmap", mJonsPackage)),

        // models
        mModelSectoid(resManifest.LoadModel("sectoid", mJonsPackage)),
        mModelCube(resManifest.LoadModel("cube", mJonsPackage)),
        mModelChair(resManifest.LoadModel("chair", mJonsPackage)),
        mModelHouse(resManifest.LoadModel("house", mJonsPackage)),
        mModelWuson(resManifest.LoadModel("wuson", mJonsPackage)),
        mModelPlane(resManifest.CreateRectangle("GroundPlane", 64, 1.0, 64)),
        mModelSphere(resManifest.CreateSphere("Sphere", 1.0f, 12, 24)),
        mModelCube2(resManifest.CreateCube("Cube2", 3)),
        mModelCube3(resManifest.CreateCube("Cube3", 3)),
		mModelBob(resManifest.LoadModel("bob", mJonsPackage)),
		mModelSAS(resManifest.LoadModel("sas", mJonsPackage)),

        // scene nodes
        mNodeSectoid(mScene.CreateSceneNode("nodeSectoid", mScene.GetRootNodeID())),
        mNodeCube(mScene.CreateSceneNode("nodeCube", mScene.GetRootNodeID())),
        mNodeChair(mScene.CreateSceneNode("nodeChair", mScene.GetRootNodeID())),
        mNodeHouse(mScene.CreateSceneNode("nodeHouse", mScene.GetRootNodeID())),
        mNodeAnimWuson(mScene.CreateSceneNode("animWuson", mScene.GetRootNodeID())),
        mNodeWuson2(mScene.CreateSceneNode("wuson2", mScene.GetRootNodeID())),
        mNodePointLight(mScene.CreateSceneNode("nodeMovingLight", mScene.GetRootNodeID())),
        mNodePlane(mScene.CreateSceneNode("nodePlane", mScene.GetRootNodeID())),
        mNodeSphere(mScene.CreateSceneNode("nodeSphere", mScene.GetRootNodeID())),
        mNodeCube2(mScene.CreateSceneNode("nodeCube2", mScene.GetRootNodeID())),
        mNodeCube3(mScene.CreateSceneNode("nodeCube3", mScene.GetRootNodeID())),
		mNodeBob(mScene.CreateSceneNode("nodeBob", mScene.GetRootNodeID())),
		mNodeSAS(mScene.CreateSceneNode("nodeSas", mScene.GetRootNodeID())),
        
        // lights
        mMovingPointLightID(mScene.CreatePointLight("MovingPointLight", mNodePointLight)),
        mSunDirLightID(mScene.CreateDirectionalLight("DirectionalLight", 4)),

        // actors
        mActorSectoid(mScene.CreateStaticActor("actorSectoid", mModelSectoid, mNodeSectoid)),
        mActorCube(mScene.CreateStaticActor("actorCube", mModelCube, mNodeCube)),
        mActorChair(mScene.CreateStaticActor("actorChair", mModelChair, mNodeChair)),
        mActorHouse(mScene.CreateStaticActor("actorHouse", mModelHouse, mNodeHouse)),
        mActorAnimWuson(mScene.CreateAnimatedActor("animWuson", mModelWuson, mNodeAnimWuson)),
        mActorWuson2(mScene.CreateStaticActor("wuson2", mModelWuson, mNodeWuson2)),
        mActorPlane(mScene.CreateStaticActor("actorPlane", mModelPlane, mNodePlane)),
        mActorSphere(mScene.CreateStaticActor("actorSphere", mModelSphere, mNodeSphere)),
        mActorCube2(mScene.CreateStaticActor("actorCube2", mModelCube2, mNodeCube2)),
        mActorCube3(mScene.CreateStaticActor("actorCube3", mModelCube3, mNodeCube3)),
		mActorAnimBob(mScene.CreateAnimatedActor("animBob", mModelBob, mNodeBob)),
		mActorSAS(mScene.CreateStaticActor("actorSAS", mModelSAS, mNodeSAS)),

        // misc animation - wuson
		mAnimWuson(INVALID_ANIMATION_ID),
        mAnimDurationWuson(0),
		mIsAnimatingWuson(false),

		// misc animaition - bob
		mAnimBob(INVALID_ANIMATION_ID),
		mAnimDurationBob(0),
		mIsAnimatingBob(false)
    {
        // ambient light
        mScene.SetAmbientLight(Vec4(0.03f));

        // sectoid
        SceneNode& nodeSectoid = mScene.GetSceneNode(mNodeSectoid);
        nodeSectoid.TranslateNode(Vec3(0.0f, 0.5f, -4.0f));

        // cube
        SceneNode& nodeCube = mScene.GetSceneNode(mNodeCube);
        nodeCube.TranslateNode(Vec3(7.0f, 1.0f, -15.0f));

        // chair
        SceneNode& nodeChair = mScene.GetSceneNode(mNodeChair);
        nodeChair.TranslateNode(Vec3(-8.0f, 0.5f, -4.0f));
        nodeChair.ScaleNode(2.0f);

        // house
        SceneNode& nodeHouse = mScene.GetSceneNode(mNodeHouse);
        nodeHouse.TranslateNode(Vec3(-7.0f, 0.5f, -15.0f));

        // animated wuson
        const Model& modelWuson = resManifest.GetModel(mModelWuson);
		mAnimWuson = modelWuson.GetAnimationID("Wuson_Run");
		const Animation& animWuson = modelWuson.GetAnimation(mAnimWuson);
		mAnimDurationWuson = animWuson.GetAnimationDuration();

        SceneNode& nodeWuson = mScene.GetSceneNode(mNodeAnimWuson);
        nodeWuson.TranslateNode(Vec3(-10.0f, 4.5f, -11.0f));
        nodeWuson.RotateNode(90.0f, Vec3(1.0f, 0.0f, 0.0f));

        // wuson 2 - not animated
        // TODO: set some other material
        StaticActor& actorWuson2 = mScene.GetStaticActor(mActorWuson2);
        //actorWuson2.SetMaterial(mMaterialCheckers);
        SceneNode& nodeWuson2 = mScene.GetSceneNode(mNodeWuson2);
        nodeWuson2.TranslateNode(Vec3(7.0f, 0.5f, 5.0f));
        nodeWuson2.RotateNode(90.0f, Vec3(1.0f, 0.0f, 0.0f));

        // point light
        PointLight& movingLight = mScene.GetPointLight(mMovingPointLightID);
        movingLight.SetRadius(10.0f);
        movingLight.SetIntensity(10.0f);
        movingLight.SetLightColor(Vec4(1.0f, 1.0f, 0.2f, 0.0f));
        SceneNode& nodePointLight = mScene.GetSceneNode(mNodePointLight);
        nodePointLight.TranslateNode(Vec3(5.0f, 3.5f, -15.0f));

        // directional light
        DirectionalLight& sun = mScene.GetDirectionalLight(mMovingPointLightID);
        sun.SetLightColor(Vec4(6.0f));

        // ground plane
        StaticActor& actorPlane = mScene.GetStaticActor(mActorPlane);
        actorPlane.SetMaterial(mMaterialTerrain);
        actorPlane.SetMaterialTilingFactor(64.0f);

        // sphere
        StaticActor& actorSphere = mScene.GetStaticActor(mActorSphere);
        actorSphere.SetMaterial(mMaterialCheckers);
        actorSphere.SetMaterialTilingFactor(3.0f);
        SceneNode& nodeSphere = mScene.GetSceneNode(mNodeSphere);
        nodeSphere.TranslateNode(Vec3(6.0f, 5.5f, 10.0f));

        // second cube
        StaticActor& actorCube2 = mScene.GetStaticActor(mActorCube2);
        actorCube2.SetMaterial(mMaterialCheckers);
        SceneNode& nodeCube2 = mScene.GetSceneNode(mNodeCube2);
        nodeCube2.TranslateNode(Vec3(3.0f, 2.0f, -15.0f));

        // third cube, far away casting shadows from the dir light
        StaticActor& actorCube3 = mScene.GetStaticActor(mActorCube3);
        actorCube3.SetMaterial(mMaterialCheckers);
        SceneNode& nodeCube3 = mScene.GetSceneNode(mNodeCube3);
        nodeCube3.TranslateNode(-sun.GetLightDirection() * 40.0f);

		// actor bob
		const Model& modelBob = resManifest.GetModel(mModelBob);
		mAnimBob = modelBob.GetAnimationID("Animation_0");
		const Animation& animBob = modelBob.GetAnimation(mAnimBob);
		mAnimDurationBob = animBob.GetAnimationDuration();

		SceneNode& nodeBob = mScene.GetSceneNode(mNodeBob);
		nodeBob.TranslateNode(Vec3(6.0f, 0.5f, -6.0f));
		nodeBob.ScaleNode(0.06f);

		// actor SAS
		SceneNode& nodeSAS = mScene.GetSceneNode(mNodeSAS);
		nodeSAS.TranslateNode(Vec3(-3.0f, 0.7f, -5.5f));
		nodeSAS.RotateNode(90.0f, Vec3(-1.0f, 0.0f, 0.0f));
		nodeSAS.ScaleNode(2.0f);

        // load skybox
        mScene.SetSkybox(mSkybox);

        // move up camera
        mScene.GetSceneCamera().TranslateCamera(Vec3(0.0f, 3.0f, 0.0f));
    }

    Scene::~Scene()
    {
        // TODO: cleanup...
    }


    void Scene::ToggleAnimationWuson()
    {
        AnimatedActor& animWuson = mScene.GetAnimatedActor(mActorAnimWuson);
		if (mIsAnimatingWuson)
			animWuson.StopAnimation();
		else
			animWuson.PlayAnimation(mAnimWuson, mAnimDurationWuson, true);

		mIsAnimatingWuson = !mIsAnimatingWuson;
    }

	void Scene::ToggleAnimationBob()
	{
		AnimatedActor& animBob = mScene.GetAnimatedActor(mActorAnimBob);
		if (mIsAnimatingBob)
			animBob.StopAnimation();
		else
			animBob.PlayAnimation(mAnimBob, mAnimDurationBob, true);

		mIsAnimatingBob = !mIsAnimatingBob;
	}


    JonsEngine::Scene& Scene::GetJonsScene()
    {
        return mScene;
    }

    JonsEngine::SceneNode& Scene::GetMovingLightNode()
    {
        return mScene.GetSceneNode(mNodePointLight);
    }

    JonsEngine::DirectionalLight& Scene::GetSun()
    {
        return mScene.GetDirectionalLight(mSunDirLightID);
    }
}