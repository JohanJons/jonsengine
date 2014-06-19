#pragma once

#include "include/Core/Types.h"

#include "GL/glew.h"
#include <array>
#include <memory>
#include <functional>

namespace JonsEngine
{
    class Logger;
    class GBuffer;

    /* GBufferPtr definition */
    typedef std::unique_ptr<GBuffer, std::function<void(GBuffer*)>> GBufferPtr;

    /* GBuffer definition */
    class GBuffer
    {
    public:
        enum GBUFFER_COLOR_ATTACHMENT
        {
            GBUFFER_COLOR_ATTACHMENT_POSITION = GL_COLOR_ATTACHMENT0,
            GBUFFER_COLOR_ATTACHMENT_NORMAL   = GL_COLOR_ATTACHMENT1,
            GBUFFER_COLOR_ATTACHMENT_DIFFUSE  = GL_COLOR_ATTACHMENT2,
            GBUFFER_NUM_GEOMETRY_ATTACHMENTS  = 3
        };

        GBuffer(Logger& logger, const uint32_t windowWidth, const uint32_t windowHeight, const uint32_t numSamples);
        ~GBuffer();

        void BindGeometryForDrawing();
        void BindGeometryForReading();
        void BindPositionForReading();
        void BindNormalsForReading();
        void BindDiffuseForReading();
        void BindDepthForReading();


    private:
        Logger& mLogger;

        std::array<GLuint, GBUFFER_NUM_GEOMETRY_ATTACHMENTS> mGBufferTextures;
        GLuint mFramebuffer;
        GLuint mDepthBuffer;
        GLuint mTextureSampler;
    };
}