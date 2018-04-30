#pragma once

#include "include/Core/EngineSettings.h"
#include "include/Scene/SceneDirtyFlags.h"
#include "include/RenderQueue/RenderQueue.h"

namespace JonsEngine
{
	struct DebugOptions;
    class ResourceManifest;
    class Scene;

    class SceneParser
    {
    public:
        SceneParser(const EngineSettings& engineSettings, const ResourceManifest& resManifest);
        ~SceneParser();

        const RenderQueue& ParseScene(const Scene& scene, DirtyFlagsSet dirtyFlags, const DebugOptions& debugOpts, const float windowAspectRatio, const float zNear, const float zFar);


    private:
		// TODO: support multiple cameras
        void ViewFrustumCulling(const Scene& scene, const float windowAspectRatio, const float zNear, const float zFar);
        void PointLightCulling(const Scene& scene);
        void DirectionalLightCulling(const Scene& scene);
		void TerrainParsing( const Scene& scene, bool bAABBDebug );
		void CopyBoneTransforms(const Scene& scene);
		void GetSkybox(const Scene& scene);
		void GetAmbientLight(const Scene& scene);
		// needs to run after ViewFrustumCulling
		void AddAABBDebugData(const Scene& scene);


        const ResourceManifest& mResourceManifest;
        const EngineSettings::CullingStrategy mCullingStrategy;
		DX11MeshID mDebugAABBMesh = INVALID_DX11_MESH_ID;
        RenderQueue mRenderQueue;
    };
}