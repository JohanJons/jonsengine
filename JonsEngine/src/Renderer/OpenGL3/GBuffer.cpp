#include "include/Renderer/OpenGL3/GBuffer.h"

#include "include/Renderer/OpenGL3/OpenGLUtils.h"
#include "include/Renderer/OpenGL3/OpenGLTexture.h"
#include "include/Core/Logging/Logger.h"

#include <exception>

namespace JonsEngine
{
    GBuffer::GBuffer(Logger& logger, const uint32_t windowWidth, const uint32_t windowHeight, const uint32_t numSamples) : mLogger(logger), mDepthBuffer(0), mTextureSampler(0)
    {
        GLCALL(glGenFramebuffers(1, &mFramebuffer));
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer));

        // color attachments to store geometry data
        GLCALL(glGenTextures(GBUFFER_NUM_GEOMETRY_ATTACHMENTS, &mGBufferTextures[0]));
        for (uint32_t index = 0; index < GBUFFER_NUM_GEOMETRY_ATTACHMENTS; index++)
        {
            GLCALL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mGBufferTextures[index]));
            GLCALL(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, numSamples, GL_RGB32F, windowWidth, windowHeight, GL_FALSE));
            GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GBUFFER_COLOR_ATTACHMENT_POSITION + index, GL_TEXTURE_2D_MULTISAMPLE, mGBufferTextures[index], 0));
        }

        // depthbuffer
        GLCALL(glGenTextures(1, &mDepthBuffer));
        GLCALL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mDepthBuffer));
        GLCALL(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, numSamples, GL_DEPTH_COMPONENT32F, windowWidth, windowHeight, GL_FALSE));
        GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, mDepthBuffer, 0));
        GLCALL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0));
        
        GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (Status != GL_FRAMEBUFFER_COMPLETE) {
            JONS_LOG_ERROR(mLogger, "GBuffer Framebuffer incomplete!");
            throw std::runtime_error("GBuffer Framebuffer incomplete!");
        }

        // color attachment texture sampler
        GLCALL(glGenSamplers(1, &mTextureSampler));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCALL(glBindSampler(OpenGLTexture::TEXTURE_UNIT_GBUFFER_POSITION, mTextureSampler));
        GLCALL(glBindSampler(OpenGLTexture::TEXTURE_UNIT_GBUFFER_NORMAL, mTextureSampler));
        GLCALL(glBindSampler(OpenGLTexture::TEXTURE_UNIT_GBUFFER_DIFFUSE, mTextureSampler));
        GLCALL(glBindSampler(OpenGLTexture::TEXTURE_UNIT_GBUFFER_DEPTH, mTextureSampler));

        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    GBuffer::~GBuffer()
    {
        GLCALL(glDeleteTextures(GBUFFER_NUM_GEOMETRY_ATTACHMENTS, &mGBufferTextures[0]));
        GLCALL(glDeleteTextures(1, &mDepthBuffer));
        GLCALL(glDeleteSamplers(1, &mTextureSampler));
        GLCALL(glDeleteFramebuffers(1, &mFramebuffer));
    }


    void GBuffer::BindGeometryForDrawing()
    {
        GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFramebuffer));

        // set geometry pass output buffers
        GLenum drawBuffers[] = { GBuffer::GBUFFER_COLOR_ATTACHMENT_POSITION, GBuffer::GBUFFER_COLOR_ATTACHMENT_NORMAL, GBuffer::GBUFFER_COLOR_ATTACHMENT_DIFFUSE };
        GLCALL(glDrawBuffers(GBuffer::GBUFFER_NUM_GEOMETRY_ATTACHMENTS, drawBuffers));
    }

    void GBuffer::BindGeometryForReading()
    {
        for (uint32_t index = 0; index < GBUFFER_NUM_GEOMETRY_ATTACHMENTS; index++)
        {
            GLCALL(glActiveTexture(GL_TEXTURE0 + OpenGLTexture::TEXTURE_UNIT_GBUFFER_POSITION + index));  // POSITION is starting texture index
            GLCALL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mGBufferTextures[index]));
        }
    }

    void GBuffer::BindPositionForReading()
    {
        GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, mFramebuffer));
        GLCALL(glReadBuffer(GBuffer::GBUFFER_COLOR_ATTACHMENT_POSITION));
    }

    void GBuffer::BindNormalsForReading()
    {
        GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, mFramebuffer));
        GLCALL(glReadBuffer(GBuffer::GBUFFER_COLOR_ATTACHMENT_NORMAL));
    }

    void GBuffer::BindDiffuseForReading()
    {
        GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, mFramebuffer));
        GLCALL(glReadBuffer(GBuffer::GBUFFER_COLOR_ATTACHMENT_DIFFUSE));
    }

    void GBuffer::BindDepthForReading()
    {
        GLCALL(glActiveTexture(GL_TEXTURE0 + OpenGLTexture::TEXTURE_UNIT_GBUFFER_DEPTH));
        GLCALL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mDepthBuffer));
    }
}