#pragma once

#include "Core/Types.h"

#include <string>
#include <bitset>

namespace JonsEngine
{
    /* DebugOptions definition */
    struct DebugOptions
    {
        enum RenderingFlag
        {
            RENDER_FLAG_DRAW_MODEL_AABB = 0,
			RENDER_FLAG_DRAW_TERRAIN_AABB,
			RENDER_FLAG_DRAW_TERRAIN_PATCH,
			RENDER_FLAG_DRAW_TERRAIN_WIREFRAME,
			RENDER_FLAG_DRAW_TERRAIN_COPLANARITY,
			RENDER_FLAG_DRAW_TERRAIN_NORMAL,
            RENDER_FLAG_DRAW_LIGHTS,
            RENDER_FLAG_SHADOWMAP_SPLITS,
			NUM_RENDER_FLAGS
        };

		typedef std::bitset<NUM_RENDER_FLAGS> RenderingFlags;


        RenderingFlags mRenderingFlags;
    };
}