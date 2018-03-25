#include "include/Scene.h"

#include "include/Core/Types.h"

using namespace JonsEngine;

constexpr float gTerrainHeightScale = 64.0f;
constexpr uint32_t gTerrainPatchSize = 64;

namespace JonsGame
{
    Scene::Scene( JonsEngine::SceneManager& sceneMgr, JonsEngine::ResourceManifest& resManifest ) :
        // base
		mID( sceneMgr.CreateScene( "DefaultScene" ) ),
		mSceneMgr( sceneMgr ), 
        mJonsPackage(ReadJonsPkg("assets.jons")),

        // skybox
        mSkybox(resManifest.LoadSkybox("skybox", mJonsPackage)),

        // materials
        mMaterialCheckers(resManifest.CreateMaterial("checkers", "checkers", "", mJonsPackage)),

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
        mNodeSectoid( GetJonsScene().CreateSceneNode("nodeSectoid", GetJonsScene().GetRootNodeID())),
        mNodeCube( GetJonsScene().CreateSceneNode("nodeCube", GetJonsScene().GetRootNodeID())),
        mNodeChair( GetJonsScene().CreateSceneNode("nodeChair", GetJonsScene().GetRootNodeID())),
        mNodeHouse( GetJonsScene().CreateSceneNode("nodeHouse", GetJonsScene().GetRootNodeID())),
        mNodeAnimWuson( GetJonsScene().CreateSceneNode("animWuson", GetJonsScene().GetRootNodeID())),
        mNodeWuson2( GetJonsScene().CreateSceneNode("wuson2", GetJonsScene().GetRootNodeID())),
        mNodePointLight( GetJonsScene().CreateSceneNode("nodeMovingLight", GetJonsScene().GetRootNodeID())),
        mNodePlane( GetJonsScene().CreateSceneNode("nodePlane", GetJonsScene().GetRootNodeID())),
        mNodeSphere( GetJonsScene().CreateSceneNode("nodeSphere", GetJonsScene().GetRootNodeID())),
        mNodeCube2( GetJonsScene().CreateSceneNode("nodeCube2", GetJonsScene().GetRootNodeID())),
        mNodeCube3( GetJonsScene().CreateSceneNode("nodeCube3", GetJonsScene().GetRootNodeID())),
		mNodeBob( GetJonsScene().CreateSceneNode("nodeBob", GetJonsScene().GetRootNodeID())),
		mNodeSAS( GetJonsScene().CreateSceneNode("nodeSas", GetJonsScene().GetRootNodeID())),
        
        // lights
        mMovingPointLightID( GetJonsScene().CreatePointLight("MovingPointLight", mNodePointLight)),
        mSunDirLightID( GetJonsScene().CreateDirectionalLight("DirectionalLight", 4)),

        // actors
        mActorSectoid( GetJonsScene().CreateStaticActor("actorSectoid", mModelSectoid, mNodeSectoid)),
        mActorCube( GetJonsScene().CreateStaticActor("actorCube", mModelCube, mNodeCube)),
        mActorChair( GetJonsScene().CreateStaticActor("actorChair", mModelChair, mNodeChair)),
        mActorHouse( GetJonsScene().CreateStaticActor("actorHouse", mModelHouse, mNodeHouse)),
        mActorAnimWuson( GetJonsScene().CreateAnimatedActor("animWuson", mModelWuson, mNodeAnimWuson)),
        mActorWuson2( GetJonsScene().CreateStaticActor("wuson2", mModelWuson, mNodeWuson2)),
        mActorPlane( GetJonsScene().CreateStaticActor("actorPlane", mModelPlane, mNodePlane)),
        mActorSphere( GetJonsScene().CreateStaticActor("actorSphere", mModelSphere, mNodeSphere)),
        mActorCube2( GetJonsScene().CreateStaticActor("actorCube2", mModelCube2, mNodeCube2)),
        mActorCube3( GetJonsScene().CreateStaticActor("actorCube3", mModelCube3, mNodeCube3)),
		mActorAnimBob( GetJonsScene().CreateAnimatedActor("animBob", mModelBob, mNodeBob)),
		mActorSAS( GetJonsScene().CreateStaticActor("actorSAS", mModelSAS, mNodeSAS)),

        // misc animation - wuson
		mAnimWuson(INVALID_ANIMATION_ID),
        mAnimDurationWuson(0),
		mIsAnimatingWuson(false),

		// misc animaition - bob
		mAnimBob(INVALID_ANIMATION_ID),
		mAnimDurationBob(0),
		mIsAnimatingBob(false)
    {
		JonsEngine::Scene& scene = sceneMgr.GetScene( mID );

        // ambient light
		scene.SetAmbientLight(Vec4(0.03f));

        // sectoid
        SceneNode& nodeSectoid = scene.GetSceneNode(mNodeSectoid);
        nodeSectoid.TranslateNode(Vec3(0.0f, 0.5f, -4.0f));

        // cube
        SceneNode& nodeCube = scene.GetSceneNode(mNodeCube);
        nodeCube.TranslateNode(Vec3(7.0f, 1.0f, -15.0f));

        // chair
        SceneNode& nodeChair = scene.GetSceneNode(mNodeChair);
        nodeChair.TranslateNode(Vec3(-8.0f, 0.5f, -4.0f));
        nodeChair.ScaleNode(2.0f);

        // house
        SceneNode& nodeHouse = scene.GetSceneNode(mNodeHouse);
        nodeHouse.TranslateNode(Vec3(-7.0f, 0.5f, -15.0f));

        // animated wuson
        const Model& modelWuson = resManifest.GetModel(mModelWuson);
		mAnimWuson = modelWuson.GetAnimationID("Wuson_Run");
		const Animation& animWuson = modelWuson.GetAnimation(mAnimWuson);
		mAnimDurationWuson = animWuson.GetAnimationDuration();

        SceneNode& nodeWuson = scene.GetSceneNode(mNodeAnimWuson);
        nodeWuson.TranslateNode(Vec3(-10.0f, 4.5f, -11.0f));
        nodeWuson.RotateNode(90.0f, Vec3(1.0f, 0.0f, 0.0f));

        // wuson 2 - not animated
        // TODO: set some other material
        StaticActor& actorWuson2 = scene.GetStaticActor(mActorWuson2);
        //actorWuson2.SetMaterial(mMaterialCheckers);
        SceneNode& nodeWuson2 = scene.GetSceneNode(mNodeWuson2);
        nodeWuson2.TranslateNode(Vec3(7.0f, 0.5f, 5.0f));
        nodeWuson2.RotateNode(90.0f, Vec3(1.0f, 0.0f, 0.0f));

        // point light
        PointLight& movingLight = scene.GetPointLight(mMovingPointLightID);
        movingLight.SetRadius(10.0f);
        movingLight.SetIntensity(10.0f);
        movingLight.SetLightColor(Vec4(1.0f, 1.0f, 0.2f, 0.0f));
        SceneNode& nodePointLight = scene.GetSceneNode(mNodePointLight);
        nodePointLight.TranslateNode(Vec3(5.0f, 3.5f, -15.0f));

        // directional light
        DirectionalLight& sun = scene.GetDirectionalLight( mSunDirLightID );
        sun.SetLightColor(Vec4(6.0f));

        // ground plane
        StaticActor& actorPlane = scene.GetStaticActor(mActorPlane);
        actorPlane.SetMaterial(mMaterialCheckers);
        actorPlane.SetMaterialTilingFactor(64.0f);

        // sphere
        StaticActor& actorSphere = scene.GetStaticActor(mActorSphere);
        actorSphere.SetMaterial(mMaterialCheckers);
        actorSphere.SetMaterialTilingFactor(3.0f);
        SceneNode& nodeSphere = scene.GetSceneNode(mNodeSphere);
        nodeSphere.TranslateNode(Vec3(6.0f, 5.5f, 10.0f));

        // second cube
        StaticActor& actorCube2 = scene.GetStaticActor(mActorCube2);
        actorCube2.SetMaterial(mMaterialCheckers);
        SceneNode& nodeCube2 = scene.GetSceneNode(mNodeCube2);
        nodeCube2.TranslateNode(Vec3(3.0f, 2.0f, -15.0f));

        // third cube, far away casting shadows from the dir light
        StaticActor& actorCube3 = scene.GetStaticActor(mActorCube3);
        actorCube3.SetMaterial(mMaterialCheckers);
        SceneNode& nodeCube3 = scene.GetSceneNode(mNodeCube3);
        nodeCube3.TranslateNode(-sun.GetLightDirection() * 40.0f);

		// actor bob
		const Model& modelBob = resManifest.GetModel(mModelBob);
		mAnimBob = modelBob.GetAnimationID("Animation_0");
		const Animation& animBob = modelBob.GetAnimation(mAnimBob);
		mAnimDurationBob = animBob.GetAnimationDuration();

		SceneNode& nodeBob = scene.GetSceneNode(mNodeBob);
		nodeBob.TranslateNode(Vec3(6.0f, 0.5f, -6.0f));
		nodeBob.ScaleNode(0.06f);

		// actor SAS
		SceneNode& nodeSAS = scene.GetSceneNode(mNodeSAS);
		nodeSAS.TranslateNode(Vec3(-3.0f, 0.7f, -5.5f));
		nodeSAS.RotateNode(90.0f, Vec3(-1.0f, 0.0f, 0.0f));
		nodeSAS.ScaleNode(2.0f);

        // load skybox
		scene.SetSkybox(mSkybox);

        // move up camera
		scene.GetSceneCamera().TranslateCamera(Vec3(0.0f, 3.0f, 0.0f));
    }

    Scene::~Scene()
    {
        // TODO: cleanup...
    }


    void Scene::ToggleAnimationWuson()
    {
        AnimatedActor& animWuson = GetJonsScene().GetAnimatedActor(mActorAnimWuson);
		if (mIsAnimatingWuson)
			animWuson.StopAnimation();
		else
			animWuson.PlayAnimation(mAnimWuson, mAnimDurationWuson, true);

		mIsAnimatingWuson = !mIsAnimatingWuson;
    }

	void Scene::ToggleAnimationBob()
	{
		AnimatedActor& animBob = GetJonsScene().GetAnimatedActor(mActorAnimBob);
		if (mIsAnimatingBob)
			animBob.StopAnimation();
		else
			animBob.PlayAnimation(mAnimBob, mAnimDurationBob, true);

		mIsAnimatingBob = !mIsAnimatingBob;
	}


    JonsEngine::Scene& Scene::GetJonsScene()
    {
        return mSceneMgr.GetScene( mID );
    }

    JonsEngine::SceneNode& Scene::GetMovingLightNode()
    {
        return GetJonsScene().GetSceneNode(mNodePointLight);
    }

    JonsEngine::DirectionalLight& Scene::GetSun()
    {
        return GetJonsScene().GetDirectionalLight(mSunDirLightID);
    }
}