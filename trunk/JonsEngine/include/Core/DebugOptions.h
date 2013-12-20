#pragma once

#include "include/Core/EngineDefs.h"

#include <string>
#include <bitset>

namespace JonsEngine
{
    /* DebugOptions definition */
    struct DebugOptions
    {
        typedef std::bitset<sizeof(uint32_t)> RenderingFlags;

        enum RenderingMode
        {
            RENDER_MODE_FULL = 0,
            RENDER_MODE_POSITIONS,
            RENDER_MODE_NORMALS,
            RENDER_MODE_DIFFUSE,
        };

        enum RenderingFlag
        {
            RENDER_FLAG_DRAW_LIGHTS
        };


        DebugOptions() : mRenderingMode(RENDER_MODE_FULL), mRenderingFlags()
        {
        }


        RenderingMode mRenderingMode;
        RenderingFlags mRenderingFlags;
    };
}