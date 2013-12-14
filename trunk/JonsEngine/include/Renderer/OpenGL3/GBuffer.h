#pragma once

#include "include/Core/Types.h"

#include "GL/glew.h"
#include <array>

namespace JonsEngine
{
    class Logger;

    struct GBuffer
    {
        enum GBUFFER_TEXTURE_TYPE
        {
            GBUFFER_TEXTURE_POSITION = 0,
            GBUFFER_TEXTURE_NORMAL,
            GBUFFER_TEXTURE_DIFFUSE,
            GBUFFER_NUM_TEXTURES
        };

        GBuffer(Logger& logger, const uint32_t shadingProgramID, const uint32_t windowWidth, const uint32_t windowHeight);
        ~GBuffer();

        Logger& mLogger;

        std::array<GLuint, GBUFFER_NUM_TEXTURES> mGBufferTextures;
        GLuint mFramebuffer;
        GLuint mDepthbuffer;
        GLuint mTextureSampler;
    };
}