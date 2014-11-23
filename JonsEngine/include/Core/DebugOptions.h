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

        enum RenderingFlag
        {
            RENDER_FLAG_DRAW_AABB = 0,
            RENDER_FLAG_DRAW_LIGHTS,
            RENDER_FLAG_SHADOWMAP_SPLITS
        };


        DebugOptions() : mRenderingFlags()
        {
        }


        RenderingFlags mRenderingFlags;
    };
}