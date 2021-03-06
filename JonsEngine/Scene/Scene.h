#pragma once

#include "Core/Types.h"
#include "Core/Containers/RangedIterator.hpp"
#include "Core/Containers/IDMap.hpp"
#include "Core/Containers/IDMapTree.hpp"
#include "Core/Utils/TimeUtils.h"
#include "Scene/AnimatedActor.h"
#include "Scene/Camera.h"
#include "Scene/DirectionalLight.h"
#include "Scene/PointLight.h"
#include "Scene/SceneNode.h"
#include "Scene/StaticActor.h"
#include "Scene/Terrain.h"
#include "Scene/AnimationUpdater.h"
#include "Scene/TerrainTransforms.h"
#include "Scene/SceneDirtyFlags.h"
#include "Resources/Skybox.h"

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
        typedef ConstRangedIterator<IDMapTree<SceneNode>> SceneNodeIterator;
        typedef ConstRangedIterator<IDMap<StaticActor>> StaticActorIterator;
        typedef ConstRangedIterator<IDMap<AnimatedActor>> AnimatedActorIterator;
        typedef ConstRangedIterator<IDMap<PointLight>> PointLightIterator;
        typedef ConstRangedIterator<IDMap<DirectionalLight>> DirectionalLightIterator;
		typedef ConstRangedIterator<IDMap<Terrain>> TerrainIterator;

        Scene( const std::string& sceneName, DX11Renderer& renderer, const ResourceManifest& resourceManifest );
		Scene( Scene&& other );

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

		TerrainID CreateTerrain(const std::string& name, float heightScale, float variationScale, SceneNodeID node, TerrainDataID terrainDataID);
		void DeleteTerrain( TerrainID& terrainDataID);
		Terrain& GetTerrain(const TerrainID terrainDataID);
		const Terrain& GetTerrain(const TerrainID terrainDataID) const;
		TerrainID GetTerrainID( const Terrain& terrain ) const; // TODO: some other fix
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
        void MarkAsDirty( SceneNode* sceneNode );
		void MarkAsDirty( Terrain* terrain );
        void UpdateDirtyObjects();
        void UpdateDirLightSplitRanges( float cameraFov, float windowAspectRatio );
        void UpdateAnimatedActors( Milliseconds elapsedTime );
		void UpdateBoneTransforms( Milliseconds elapsedTime );


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

	typedef IDMap<Scene>::ItemID SceneID;
	static const SceneID INVALID_SCENE_ID = IDMap<Scene>::INVALID_ITEM_ID;
}