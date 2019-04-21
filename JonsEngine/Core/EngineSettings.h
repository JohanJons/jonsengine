#pragma once

#include "Renderer/RenderSettings.h"
#include "Window/WindowSettings.h"
#include "Scene/SceneSettings.h"

namespace JonsEngine
{
    /*
     * EngineSettings is used when bootstraping the engine. 
     * Parameters can be changed to alter the engine properties
     */

    struct EngineSettings : RenderSettings, WindowSettings, SceneSettings
    {
    };
}