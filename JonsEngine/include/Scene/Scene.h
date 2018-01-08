#pragma once

#include "include/Core/Types.h"
#include "include/Core/Containers/RangedIterator.hpp"
#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Containers/IDMapTree.hpp"
#include "include/Core/Utils/Time.h"
#include "include/Scene/AnimatedActor.h"
#include "include/Scene/Camera.h"
#include "include/Scene/DirectionalLight.h"
#include "include/Scene/PointLight.h"
#include "include/Scene/SceneNode.h"
#include "include/Scene/StaticActor.h"
#include "include/Scene/Terrain.h"
#include "include/Scene/AnimationUpdater.h"
#include "include/Scene/TerrainTransforms.h"
#include "include/Scene/SceneDirtyFlags.h"
#include "include/Resources/Skybox.h"

#include <string>
#include <vector>
#include <memory>

namespace JonsEngine
{
    class DX11Renderer;
    class ResourceManifest;

    class Scene
    {
    public:
        typedef ConstRangedIterator<IDMapTree<SceneNode>, IDMapTree<SceneNode>::const_iterator> SceneNodeIterator;
        typedef ConstRangedIterator<IDMap<StaticActor>> StaticActorIterator;
        typedef ConstRangedIterator<IDMap<AnimatedActor>> AnimatedActorIterator;
        typedef ConstRangedIterator<IDMap<PointLight>> PointLightIterator;
        typedef ConstRangedIterator<IDMap<DirectionalLight>> DirectionalLightIterator;
		typedef ConstRangedIterator<IDMap<Terrain>> TerrainIterator;

        Scene(const std::string& sceneName, DX11Renderer& renderer, const ResourceManifest& resourceManifest);
		~Scene() = default;

        void Tick(Milliseconds elapsedTime, float windowAspectRatio);
		DirtyFlagsSet GetAndResetDirtyFlags();

        SceneNode& GetRootNode();
        const SceneNodeID GetRootNodeID() const;

        SceneNodeID CreateSceneNode(const std::string& sceneNodeName, SceneNodeID parent);
        void DeleteSceneNode(SceneNodeID& sceneNodeID);
        SceneNode& GetSceneNode(SceneNodeID sceneNodeID);
        const SceneNode& GetSceneNode(SceneNodeID sceneNodeID) const;
        SceneNodeIterator GetSceneNodes() const;

        StaticActorID CreateStaticActor(const std::string& actorName, ModelID modelID, SceneNodeID sceneNodeID);
        void DeleteStaticActor(StaticActorID& actorID);
        StaticActor& GetStaticActor(StaticActorID actorID);
        const StaticActor& GetStaticActor(StaticActorID actorID) const;
        StaticActorIterator GetStaticActors() const;

        AnimatedActorID CreateAnimatedActor(const std::string& actorName, ModelID modelID, SceneNodeID sceneNodeID);
        void DeleteAnimatedActor(AnimatedActorID& actorID);
        AnimatedActor& GetAnimatedActor(AnimatedActorID actorID);
        const AnimatedActor& GetAnimatedActor(AnimatedActorID actorID) const;
        AnimatedActorIterator GetAnimatedActors() const;

        PointLightID CreatePointLight(const std::string& lightName, SceneNodeID sceneNodeID);
        void DeletePointLight(PointLightID& pointLightID);
		PointLight& GetPointLight(PointLightID pointLightID);
        const PointLight& GetPointLight(PointLightID pointLightID) const;
        PointLightIterator GetPointLights() const;
        
        DirectionalLightID CreateDirectionalLight(const std::string& lightName);
        DirectionalLightID CreateDirectionalLight(const std::string& lightName, uint32_t numShadowmapCascades);
        void DeleteDirectionalLight(DirectionalLightID& dirLightID);
        DirectionalLight& GetDirectionalLight(DirectionalLightID dirLightID);
        const DirectionalLight& GetDirectionalLight(DirectionalLightID dirLightID) const;
        DirectionalLightIterator GetDirectionalLights() const;

		TerrainID CreateTerrain(const std::string& name, float heightScale, uint32_t patchSize, SceneNodeID node, TerrainDataID terrainDataID);
		void DeleteTerrain(TerrainDataID& terrainDataID);
		Terrain& GetTerrain(const TerrainDataID terrainDataID);
		const Terrain& GetTerrain(const TerrainDataID terrainDataID) const;
		TerrainIterator GetTerrains() const;

		void SetAmbientLight(const Vec4& ambientLight);
        const Vec4& GetAmbientLight() const;

        void SetSkybox(SkyboxID skyboxID);
        SkyboxID GetSkybox() const;

        Camera& GetSceneCamera();
        const Camera& GetSceneCamera() const;

		const AnimationUpdater& GetAnimationUpdater() const { return mAnimationUpdater; }
		const TerrainTransforms& GetTerrainTransforms() const { return mTerrainTransforms; }

		const std::string mName;


    private:
        void MarkAsDirty(SceneNode* sceneNode);
        void UpdateDirtyObjects();
        void UpdateDirLightSplitRanges(float cameraFov, float windowAspectRatio);
        void UpdateAnimatedActors(Milliseconds elapsedTime);
		void UpdateBoneTransforms(Milliseconds elapsedTime);


        DX11Renderer& mRenderer;
        const ResourceManifest& mResourceManifest;

        AnimationUpdater mAnimationUpdater;
		Camera mSceneCamera;
        Vec4 mAmbientLight;
        SkyboxID mSkyboxID;

		IDMap<PointLight> mPointLights;
        IDMap<DirectionalLight> mDirectionalLights;
		IDMap<Terrain> mTerrains;
        IDMap<StaticActor> mStaticActors;
        IDMap<AnimatedActor> mAnimatedActors;
        IDMapTree<SceneNode> mSceneNodeTree;

		TerrainTransforms mTerrainTransforms;

		DirtyFlagsSet mDirtyFlags;
        bool mHasDirtySceneNodes;
        const SceneNodeID mRootNodeID;
    };
}