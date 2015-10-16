#pragma once

#include "include/Renderer/RenderQueue.h"

namespace JonsEngine
{
    class ResourceManifest;
    class Scene;

    class SceneParser
    {
    public:
        SceneParser(const ResourceManifest& resManifest);
        ~SceneParser();

        const RenderQueue& ParseScene(const Scene& scene);


    private:
        const ResourceManifest& mResourceManifest;
        RenderQueue mRenderQueue;
    };
}