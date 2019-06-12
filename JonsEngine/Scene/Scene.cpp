#include "Scene/Scene.h"
#include "Resources/ResourceManifest.h"
#include "Renderer/DirectX11/DX11Renderer.h"
#include "Renderer/RenderDefs.h"

#include <algorithm>
#include <functional>

namespace JonsEngine
{
    Scene::Scene(const std::string& sceneName, DX11Renderer& renderer, const ResourceManifest& resourceManifest) :
        mName( sceneName ),
        mRenderer( renderer ),
        mResourceManifest( resourceManifest ),
        mAnimationUpdater( resourceManifest ),
        mAmbientLight( 0.2f ),
        mSkyboxID( INVALID_SKYBOX_ID ),
        mSceneNodeTree( "Root", INVALID_SCENE_NODE_ID, std::bind( static_cast<void( Scene::* )( SceneNode* sceneNode )> ( &Scene::MarkAsDirty ), this, std::placeholders::_1 ) ),
		mTerrainTransforms( mTerrains, mResourceManifest.GetTerrainDataMap(), mSceneNodeTree ),
        mRootNodeID( mSceneNodeTree.GetRootNodeID() )
    {
    }

	Scene::Scene( Scene&& other ) :
		mName( std::move( other.mName )),
		mRenderer( other.mRenderer ),
		mResourceManifest( other.mResourceManifest ),
		mAnimationUpdater( std::move( other.mAnimationUpdater ) ),
		mSceneCamera( std::move( other.mSceneCamera )),
		mAmbientLight( std::move( other.mAmbientLight ) ),
		mSkyboxID( std::move( other.mSkyboxID ) ),
		mPointLights( std::move( other.mPointLights )),
		mDirectionalLights( std::move( other.mDirectionalLights ) ),
		mTerrains( std::move( other.mTerrains ) ),
		mStaticActors( std::move( other.mStaticActors ) ),
		mAnimatedActors( std::move( other.mAnimatedActors ) ),
		mSceneNodeTree( std::move( other.mSceneNodeTree ) ),
		mTerrainTransforms( std::move( other.mTerrainTransforms )),
		mDirtyFlags( std::move( other.mDirtyFlags )),
		mHasDirtySceneNodes( other.mHasDirtySceneNodes ),
		mRootNodeID( std::move( other.mRootNodeID ))
	{
		// the objects with callbacks to the scene needs special care

		auto onPlayAnim = std::bind( &AnimationUpdater::PlayAnimation, &mAnimationUpdater, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
		auto onRestartAnim = std::bind( &AnimationUpdater::RestartAnimation, &mAnimationUpdater, std::placeholders::_1 );
		auto onStopAnim = std::bind( &AnimationUpdater::StopAnimation, &mAnimationUpdater, std::placeholders::_1 );
		for ( AnimatedActor& actor : mAnimatedActors )
			actor.SetCallbacks( onPlayAnim, onRestartAnim, onStopAnim );

		auto onNodeDirty = std::bind( static_cast<void( Scene::* )( SceneNode* sceneNode )> ( &Scene::MarkAsDirty ), this, std::placeholders::_1 );
		for ( SceneNode& node : mSceneNodeTree )
			node.SetCallback( onNodeDirty );

		auto onTerrainDirty = std::bind( static_cast<void( Scene::* )( Terrain* terrain )> ( &Scene::MarkAsDirty ), this, std::placeholders::_1 );
		for ( Terrain& terrain : mTerrains )
			terrain.SetCallback( onTerrainDirty );
	}


    void Scene::Tick(const Milliseconds elapsedTime, const float windowAspectRatio)
    {
        const float cameraFov = GetSceneCamera().GetFOV();

        UpdateDirtyObjects();
        UpdateDirLightSplitRanges(cameraFov, windowAspectRatio);
        UpdateAnimatedActors(elapsedTime);
		UpdateBoneTransforms(elapsedTime);

        uint32_t patchSize = RenderSettingsToVal( mRenderer.GetTerrainPatchSize() );
		uint32_t updatedTransforms = mTerrainTransforms.UpdateTransforms( patchSize );
		if ( updatedTransforms )
			mDirtyFlags.set(FlagTerrain, true);
    }

	DirtyFlagsSet Scene::GetAndResetDirtyFlags()
	{
		DirtyFlagsSet set(mDirtyFlags);
		mDirtyFlags.reset();
		return set;
	}


    SceneNode& Scene::GetRootNode()
    {
        return mSceneNodeTree.GetNode(mRootNodeID);
    }

    const SceneNodeID Scene::GetRootNodeID() const
    {
        return mRootNodeID;
    }

    
    SceneNodeID Scene::CreateSceneNode(const std::string& sceneNodeName, const SceneNodeID parent)
    {
        return mSceneNodeTree.AddNode(parent, sceneNodeName, parent, std::bind( static_cast<void( Scene::* )( SceneNode* sceneNode )> ( &Scene::MarkAsDirty ), this, std::placeholders::_1 ));
    }

    void Scene::DeleteSceneNode(SceneNodeID& sceneNodeID)
    {
        assert(sceneNodeID != INVALID_SCENE_NODE_ID);

        mSceneNodeTree.FreeNode(sceneNodeID);
        sceneNodeID = INVALID_SCENE_NODE_ID;
    }

    SceneNode& Scene::GetSceneNode(const SceneNodeID sceneNodeID)
    {
        return mSceneNodeTree.GetNode(sceneNodeID);
    }

    const SceneNode& Scene::GetSceneNode(const SceneNodeID sceneNodeID) const
    {
        return mSceneNodeTree.GetNode(sceneNodeID);
    }

    Scene::SceneNodeIterator Scene::GetSceneNodes() const
    {
        return SceneNodeIterator( mSceneNodeTree, 0, mSceneNodeTree.Size() );
    }


    StaticActorID Scene::CreateStaticActor(const std::string& actorName, const ModelID modelID, const SceneNodeID sceneNodeID)
    {
        return mStaticActors.Insert(actorName, modelID, sceneNodeID);
    }

    void Scene::DeleteStaticActor(StaticActorID& actorID)
    {
        assert(actorID != INVALID_STATIC_ACTOR_ID);

        mStaticActors.Erase(actorID);
        actorID = INVALID_STATIC_ACTOR_ID;
    }

    StaticActor& Scene::GetStaticActor(const StaticActorID actorID)
    {
        return mStaticActors.GetItem(actorID);
    }

    const StaticActor& Scene::GetStaticActor(const StaticActorID actorID) const
    {
        return mStaticActors.GetItem(actorID);
    }

    Scene::StaticActorIterator Scene::GetStaticActors() const
    {
        return StaticActorIterator( mStaticActors, 0, mStaticActors.Size() );
    }


    AnimatedActorID Scene::CreateAnimatedActor(const std::string& actorName, const ModelID modelID, const SceneNodeID sceneNodeID)
    {
		auto onPlayAnim = std::bind(&AnimationUpdater::PlayAnimation, &mAnimationUpdater, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		auto onRestartAnim = std::bind(&AnimationUpdater::RestartAnimation, &mAnimationUpdater, std::placeholders::_1);
        auto onStopAnim = std::bind(&AnimationUpdater::StopAnimation, &mAnimationUpdater, std::placeholders::_1);

        return mAnimatedActors.Insert(onPlayAnim, onRestartAnim, onStopAnim, actorName, modelID, sceneNodeID);
    }

    void Scene::DeleteAnimatedActor(AnimatedActorID& actorID)
    {
        assert(actorID != INVALID_ANIMATED_ACTOR_ID);

        mAnimatedActors.Erase(actorID);
        actorID = INVALID_ANIMATED_ACTOR_ID;
    }

    AnimatedActor& Scene::GetAnimatedActor(const AnimatedActorID actorID)
    {
        return mAnimatedActors.GetItem(actorID);
    }

    const AnimatedActor& Scene::GetAnimatedActor(const AnimatedActorID actorID) const
    {
        return mAnimatedActors.GetItem(actorID);
    }

    Scene::AnimatedActorIterator Scene::GetAnimatedActors() const
    {
        return AnimatedActorIterator( mAnimatedActors, 0, mAnimatedActors.Size() );
    }


	PointLightID Scene::CreatePointLight(const std::string& lightName, const SceneNodeID node)
    {
        return mPointLights.Insert(lightName, node);
    }
    
	void Scene::DeletePointLight(PointLightID& pointLightID)
    {
        assert(pointLightID != INVALID_POINT_LIGHT_ID);

		mPointLights.Erase(pointLightID);
        pointLightID = INVALID_POINT_LIGHT_ID;
    }
    
	PointLight& Scene::GetPointLight(const PointLightID pointLightID)
    {
		return mPointLights.GetItem(pointLightID);
    }

    const PointLight& Scene::GetPointLight(const PointLightID pointLightID) const
    {
        return mPointLights.GetItem(pointLightID);
    }

    Scene::PointLightIterator Scene::GetPointLights() const
    {
        return PointLightIterator( mPointLights, 0, mPointLights.Size() );
    }
    

    DirectionalLightID Scene::CreateDirectionalLight(const std::string& lightName)
    {
        const uint32_t defaultNumCascades = 4;
        assert(defaultNumCascades <= gMaxNumShadowmapCascades);

		return CreateDirectionalLight( lightName, defaultNumCascades );
    }

    DirectionalLightID Scene::CreateDirectionalLight(const std::string& lightName, const uint32_t numShadowmapCascades)
    {
        if (numShadowmapCascades > gMaxNumShadowmapCascades)
            return mDirectionalLights.INVALID_ITEM_ID;

        return mDirectionalLights.Insert(lightName, numShadowmapCascades);
    }

    void Scene::DeleteDirectionalLight(DirectionalLightID& dirLightID)
    {
        assert(dirLightID != INVALID_DIRECTIONAL_LIGHT_ID);

        mDirectionalLights.Erase(dirLightID);
        dirLightID = INVALID_DIRECTIONAL_LIGHT_ID;
    }

    DirectionalLight& Scene::GetDirectionalLight(const DirectionalLightID dirLightID)
    {
        return mDirectionalLights.GetItem(dirLightID);
    }

    const DirectionalLight& Scene::GetDirectionalLight(const DirectionalLightID dirLightID) const
    {
        return mDirectionalLights.GetItem(dirLightID);
    }

    Scene::DirectionalLightIterator Scene::GetDirectionalLights() const
    {
        return DirectionalLightIterator( mDirectionalLights, 0, mDirectionalLights.Size() );
    }


	TerrainID Scene::CreateTerrain(const std::string& name, float heightScale, float variationScale, const SceneNodeID node, const TerrainDataID terrainDataID)
	{
		TerrainID ID = mTerrains.Insert( name, heightScale, variationScale, node, terrainDataID, std::bind( static_cast<void( Scene::* )( Terrain* terrain )> ( &Scene::MarkAsDirty ), this, std::placeholders::_1 ) );
		mTerrainTransforms.AddDirty( ID );

		return ID;
	}

	void Scene::DeleteTerrain( TerrainID& terrainID )
	{
		assert( terrainID != INVALID_TERRAIN_ID);

		mTerrains.Erase( terrainID );
		terrainID = INVALID_TERRAIN_ID;
	}

	Terrain& Scene::GetTerrain(const TerrainID terrainID )
	{
		return mTerrains.GetItem( terrainID );
	}

	const Terrain& Scene::GetTerrain(const TerrainID terrainID ) const
	{
		return mTerrains.GetItem( terrainID );
	}

	Scene::TerrainIterator Scene::GetTerrains() const
	{
		return TerrainIterator( mTerrains, 0, mTerrains.Size() );
	}


	void Scene::SetAmbientLight(const Vec4& ambientLight)
    {
        mAmbientLight = ambientLight;
    }

    const Vec4& Scene::GetAmbientLight() const
    {
        return mAmbientLight;
    }


    void Scene::SetSkybox(const SkyboxID skybox)
    {
        mSkyboxID = skybox;
    }

    SkyboxID Scene::GetSkybox() const
    {
        return mSkyboxID;
    }


	Camera& Scene::GetSceneCamera()
	{ 
		return mSceneCamera;
	}

    const Camera& Scene::GetSceneCamera() const
    {
        return mSceneCamera;
    }


    void Scene::MarkAsDirty( SceneNode* sceneNode )
    {
        mHasDirtySceneNodes = true;
    }

	void Scene::MarkAsDirty( Terrain* terrain )
	{
		TerrainID id = mTerrains.GetID( *terrain );
		assert( id != INVALID_TERRAIN_ID );

		mTerrainTransforms.AddDirty( id );
        mDirtyFlags.set( FlagTerrain, true );
	}


    void Scene::UpdateDirtyObjects()
    {
        if (!mHasDirtySceneNodes)
            return;

        // special case: root has no parent
        SceneNode& rootNode = mSceneNodeTree.GetNode(mRootNodeID);
        rootNode.UpdateWorldMatrix();

        // skips root
        for (auto nodeIter = mSceneNodeTree.begin() + 1; nodeIter != mSceneNodeTree.end(); ++nodeIter)
        {
            SceneNode& node = *nodeIter;
            SceneNode& parent = mSceneNodeTree.GetNode( node.GetParentID() );
            const Mat4& parentWorldTransform = parent.GetWorldTransform();

            node.UpdateWorldMatrix( parentWorldTransform );
        }

        mHasDirtySceneNodes = false;
    }

    void Scene::UpdateDirLightSplitRanges(const float cameraFov, const float windowAspectRatio)
    {
        const Mat4 viewMatrix = GetSceneCamera().GetCameraTransform();
        float minCameraDepth = 0.01f;
        float maxCameraDepth = 1.0f;
    
        mRenderer.ReduceDepth(minCameraDepth, maxCameraDepth);
        for (DirectionalLight& dirLight : mDirectionalLights)
            dirLight.UpdateCascadesBoundingVolume(viewMatrix, cameraFov, windowAspectRatio, minCameraDepth, maxCameraDepth);
    }
    
    void Scene::UpdateAnimatedActors(const Milliseconds elapsedTime)
    {
        for (AnimatedActor& actor : mAnimatedActors)
        {
            if (!actor.IsPlaying())
                continue;
            
            actor.UpdateTimestamp(elapsedTime);
        }
    }

	void Scene::UpdateBoneTransforms(const Milliseconds elapsedTime)
	{
		mAnimationUpdater.Update(elapsedTime);
	}
}