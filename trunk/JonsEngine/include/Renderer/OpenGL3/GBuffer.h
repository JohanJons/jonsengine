#pragma once

#include "include/Core/Types.h"

#include "GL/glew.h"
#include <array>

namespace JonsEngine
{
    class Logger;

    struct GBuffer
    {
        enum GBUFFER_COLOR_ATTACHMENT
        {
            GBUFFER_COLOR_ATTACHMENT_POSITION = GL_COLOR_ATTACHMENT0,
            GBUFFER_COLOR_ATTACHMENT_NORMAL   = GL_COLOR_ATTACHMENT1,
            GBUFFER_COLOR_ATTACHMENT_DIFFUSE  = GL_COLOR_ATTACHMENT2,
            GBUFFER_NUM_GEOMETRY_ATTACHMENTS  = 3,
            GBUFFER_COLOR_ATTACHMENT_FINAL    = GL_COLOR_ATTACHMENT3
        };

        GBuffer(Logger& logger, const uint32_t windowWidth, const uint32_t windowHeight);
        ~GBuffer();

        void BindGeometryTextures();
        void BindDepthTexture();


        Logger& mLogger;

        std::array<GLuint, GBUFFER_NUM_GEOMETRY_ATTACHMENTS> mGBufferTextures;
        GLuint mFinalTexture;
        GLuint mFrameBuffer;
        GLuint mDepthBuffer;
        GLuint mTextureSampler;
    };
}