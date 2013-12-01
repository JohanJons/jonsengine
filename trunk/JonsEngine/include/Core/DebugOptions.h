#pragma once

#include "include/Core/EngineDefs.h"

#include <string>

namespace JonsEngine
{
    /* DebugOptions definition */
    struct DebugOptions
    {
        enum RenderingMode
        {
            RENDER_DEBUG_NONE = 0,
            RENDER_DEBUG_POSITIONS,
            RENDER_DEBUG_NORMALS,
            RENDER_DEBUG_TEXCOORDS,
            RENDER_DEBUG_COLOR
        };

        DebugOptions() : mRenderingMode(RENDER_DEBUG_NONE)
        {
        }

        RenderingMode mRenderingMode;
    };
}