#pragma once

#include "include/Core/EngineSettings.h"
#include "include/RenderQueue/RenderQueue.h"

namespace JonsEngine
{
    class ResourceManifest;
    class Scene;

    class SceneParser
    {
    public:
        SceneParser(const EngineSettings& engineSettings, const ResourceManifest& resManifest);
        ~SceneParser();

        const RenderQueue& ParseScene(const Scene& scene, const float windowAspectRatio, const float zNear, const float zFar);


    private:
        void ViewFrustumCulling(const Scene& scene, const float windowAspectRatio, const float zNear, const float zFar);
        void PointLightCulling(const Scene& scene);
        void DirectionalLightCulling(const Scene& scene);
        void ParseMiscSceneInfo(const Scene& scene);


        const ResourceManifest& mResourceManifest;
        const EngineSettings::CullingStrategy mCullingStrategy;
        RenderQueue mRenderQueue;
    };
}