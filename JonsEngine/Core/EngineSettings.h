#pragma once

#include "RenderSettings.h"
#include "WindowSettings.h"
#include "SceneSettings.h"

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