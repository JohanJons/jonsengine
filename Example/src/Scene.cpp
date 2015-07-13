#include "include/Scene.h"

#include "include/Core/Types.h"
#include "include/Scene/Scene.h"

using namespace JonsEngine;

namespace JonsGame
{
    Scene::Scene(JonsEngine::Scene& jonsScene, JonsEngine::ResourceManifest& resManifest, const std::string& jonsPkgName) :
        mScene(jonsScene), mMovingPointLightID(0), mSunDirLightID(0)
    {
        JonsPackagePtr jonsPackage = ReadJonsPkg("assets.jons");

        // ambient light
        mScene.SetAmbientLight(Vec4(0.01f));

        // sectoid
        SceneNodePtr nodeAlien = mScene.GetRootNode().CreateChildNode("nodeSectoid");
        ModelPtr modelAlien = resManifest.LoadModel("sectoid", jonsPackage);
        Actor* actorAlien = mScene.CreateActor("actorSectoid", modelAlien, nodeAlien);
        nodeAlien->TranslateNode(Vec3(0.0f, 0.5f, -4.0f));
        nodeAlien->RotateNode(90.0f, Vec3(1.0f, 0.0f, 0.0f));

        // cube
        SceneNodePtr nodeCube = mScene.GetRootNode().CreateChildNode("nodeCube");
        ModelPtr modelCube = resManifest.LoadModel("cube", jonsPackage);
        Actor* actorCube = mScene.CreateActor("actorCube", modelCube, nodeCube);
        nodeCube->TranslateNode(Vec3(7.0f, 1.0f, -15.0f));

        // chair
        SceneNodePtr nodeChair = mScene.GetRootNode().CreateChildNode("nodeChair");
        ModelPtr modelChair = resManifest.LoadModel("chair", jonsPackage);
        Actor* actorChair = mScene.CreateActor("actorChair", modelChair, nodeChair);
        nodeChair->TranslateNode(Vec3(-8.0f, 0.5f, -4.0f));
        nodeChair->ScaleNode(2.0f);

        // house
        SceneNodePtr nodeHouse = mScene.GetRootNode().CreateChildNode("nodeHouse");
        ModelPtr modelHouse = resManifest.LoadModel("house", jonsPackage);
        Actor* actorHouse = mScene.CreateActor("actorHouse", modelHouse, nodeHouse);
        nodeHouse->TranslateNode(Vec3(-7.0f, 0.5f, -15.0f));

        // point light
        SceneNodePtr nodeMovingLight = mScene.GetRootNode().CreateChildNode("nodeMovingLight");
        mMovingPointLightID = mScene.CreatePointLight("MovingPointLight", nodeMovingLight);
        PointLight& movingLight = mScene.GetPointLight(mMovingPointLightID);
        movingLight.mLightRadius = 10.0f;
        movingLight.mLightIntensity = 2.0f;
        movingLight.mLightColor = Vec4(1.0f, 1.0f, 0.0f, 0.0f);
        nodeMovingLight->TranslateNode(Vec3(5.0f, 3.5f, -15.0f));

        // directional light
        mSunDirLightID = mScene.CreateDirectionalLight("DirectionalLight", 4);
        DirectionalLight& sun = mScene.GetDirectionalLight(mMovingPointLightID);
        sun.mLightColor = Vec4(0.55f);

        // load checker material
        MaterialPtr checkerMaterial = resManifest.LoadMaterial("checkers", jonsPackage);

        // create a ground plane
        SceneNodePtr nodePlane = mScene.GetRootNode().CreateChildNode("nodePlane");
        ModelPtr plane = resManifest.CreateRectangle("GroundPlane", 64, 1.0, 64, checkerMaterial);
        plane->GetRootNode().GetMeshes()[0].SetTextureTilingFactor(64.0f);
        Actor* actorPlane = mScene.CreateActor("actorPlane", plane, nodePlane);

        // create a sphere
        SceneNodePtr nodeSphere = mScene.GetRootNode().CreateChildNode("nodeSphere");
        ModelPtr sphere = resManifest.CreateSphere("Sphere", 1.0f, 12, 24, checkerMaterial);
        Actor* actorSphere = mScene.CreateActor("actorSphere", sphere, nodeSphere);
        nodeSphere->TranslateNode(Vec3(6.0f, 5.5f, 10.0f));

        // create a  second cube
        SceneNodePtr nodeCube2 = mScene.GetRootNode().CreateChildNode("nodeCube2");
        ModelPtr cube2 = resManifest.CreateCube("Cube2", 3, checkerMaterial);
        Actor* actorCube2 = mScene.CreateActor("actorCube2", cube2, nodeCube2);
        nodeCube2->TranslateNode(Vec3(3.0f, 2.0f, -15.0f));

        // create a third cube, far away casting shadows from the dir light
        SceneNodePtr nodeCube3 = mScene.GetRootNode().CreateChildNode("nodeCube3");
        ModelPtr cube3 = resManifest.CreateCube("Cube3", 3, checkerMaterial);
        Actor* actorCube3 = mScene.CreateActor("actorCube3", cube3, nodeCube3);
        nodeCube3->TranslateNode(-sun.mLightDirection * 40.0f);

        // load skybox
        const SkyboxPtr skybox = resManifest.LoadSkybox("skybox", jonsPackage);
        mScene.SetSkybox(skybox);

        // move up camera
        mScene.GetSceneCamera().TranslateCamera(Vec3(0.0f, 3.0f, 0.0f));
    }

    Scene::~Scene()
    {
    }


    JonsEngine::Scene& Scene::GetJonsScene()
    {
        return mScene;
    }

    JonsEngine::PointLight& Scene::GetMovingPointLight()
    {
        return mScene.GetPointLight(mMovingPointLightID);
    }

    JonsEngine::DirectionalLight& Scene::GetSunDirLight()
    {
        return mScene.GetDirectionalLight(mSunDirLightID);
    }
}