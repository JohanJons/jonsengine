#include "include/Scene/SceneParser.h"

#include "include/Resources/ResourceManifest.h"

namespace JonsEngine
{
    SceneParser::SceneParser(const ResourceManifest& resManifest) : mResourceManifest(resManifest)
    {
    }

    SceneParser::~SceneParser()
    {
    }


    const RenderQueue& SceneParser::ParseScene(const Scene& scene)
    {
        return mRenderQueue;
    }
}