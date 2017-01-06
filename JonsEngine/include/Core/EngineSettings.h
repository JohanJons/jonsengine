#pragma once

#include "include/Renderer/RenderSettings.h"
#include "include/Window/WindowSettings.h"
#include "include/Scene/SceneSettings.h"

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