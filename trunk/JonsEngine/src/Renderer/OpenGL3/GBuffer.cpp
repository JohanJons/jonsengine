#include "include/Renderer/OpenGL3/GBuffer.h"

#include "include/Renderer/OpenGL3/OpenGLUtils.h"
#include "include/Renderer/OpenGL3/OpenGLTexture.h"
#include "include/Core/Logging/Logger.h"

#include <exception>

namespace JonsEngine
{
    GBuffer::GBuffer(Logger& logger, const uint32_t shadingProgramID, const uint32_t windowWidth, const uint32_t windowHeight) : mLogger(logger), mFramebuffer(0), mDepthbuffer(0), mTextureSampler(0)
    {
        GLCALL(glGenFramebuffers(1, &mFramebuffer));
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer));
        GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));

        // color attachments to store geometry data
        GLCALL(glGenTextures(GBUFFER_NUM_TEXTURES, &mGBufferTextures[0]));
        for (uint32_t index = 0; index < GBUFFER_NUM_TEXTURES;  index++)
        {
            GLCALL(glBindTexture(GL_TEXTURE_2D, mGBufferTextures[index]));
            GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, 0));
            GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, mGBufferTextures[index], 0));
        }
        GLCALL(glBindTexture(GL_TEXTURE_2D, 0));

        // set attachment points
        GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        GLCALL(glDrawBuffers(GBUFFER_NUM_TEXTURES, drawBuffers));

        // depthbuffer
        GLCALL(glGenRenderbuffers(1, &mDepthbuffer));
        GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, mDepthbuffer));
        GLCALL(glRenderbufferStorageMultisample(GL_RENDERBUFFER, 0, GL_DEPTH_COMPONENT32F, windowWidth, windowHeight));
        GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthbuffer));
        GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
        
        GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (Status != GL_FRAMEBUFFER_COMPLETE) {
            JONS_LOG_ERROR(mLogger, "Framebuffer incomplete!");
            throw std::runtime_error("Framebuffer incomplete!");
        }

        // color attachment texture sampler
        GLCALL(glUseProgram(shadingProgramID));
        GLCALL(glGenSamplers(1, &mTextureSampler));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCALL(glBindSampler(OpenGLTexture::TEXTURE_UNIT_GBUFFER_POSITION, mTextureSampler));
        GLCALL(glBindSampler(OpenGLTexture::TEXTURE_UNIT_GBUFFER_NORMAL, mTextureSampler));
        GLCALL(glBindSampler(OpenGLTexture::TEXTURE_UNIT_GBUFFER_DIFFUSE, mTextureSampler));
        GLCALL(glUseProgram(0));

        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    GBuffer::~GBuffer()
    {
        GLCALL(glDeleteTextures(GBUFFER_NUM_TEXTURES, &mGBufferTextures[0]));
        GLCALL(glDeleteRenderbuffers(1, &mDepthbuffer));
        GLCALL(glDeleteSamplers(1, &mTextureSampler));
        GLCALL(glDeleteFramebuffers(1, &mFramebuffer));
    }
}