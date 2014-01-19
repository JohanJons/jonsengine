#pragma once

#include "include/Renderer/OpenGL3/DirectionalShadowMap.h"

#include "include/Renderer/OpenGL3/OpenGLTexture.h"
#include "include/Renderer/OpenGL3/OpenGLUtils.h"
#include "include/Core/Logging/Logger.h"

namespace JonsEngine
{
    DirectionalShadowMap::DirectionalShadowMap(Logger& logger, const uint32_t windowWidth, const uint32_t windowHeight) : mLogger(logger)
    {
        GLCALL(glGenFramebuffers(1, &mFramebuffer));
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer));

        // shadow map texture
        GLCALL(glGenTextures(1, &mShadowMap));
        GLCALL(glBindTexture(GL_TEXTURE_2D, mShadowMap));
        GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0));
        GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mShadowMap, 0));
        GLCALL(glBindTexture(GL_TEXTURE_2D, 0));

        GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (Status != GL_FRAMEBUFFER_COMPLETE) {
            JONS_LOG_ERROR(mLogger, "DirectionalShadowMap Framebuffer incomplete!");
            throw std::runtime_error("DirectionalShadowMap Framebuffer incomplete!");
        }

        // color attachment texture sampler
        GLCALL(glGenSamplers(1, &mTextureSampler));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCALL(glBindSampler(OpenGLTexture::TEXTURE_UNIT_SHADOW_DIRECTIONAL, mTextureSampler));
        GLCALL(glUseProgram(0));

        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    DirectionalShadowMap::~DirectionalShadowMap()
    {
        GLCALL(glDeleteTextures(1, &mShadowMap));
        GLCALL(glDeleteSamplers(1, &mTextureSampler));
        GLCALL(glDeleteFramebuffers(1, &mFramebuffer));
    }


    void DirectionalShadowMap::BindShadowMap()
    {
        GLCALL(glActiveTexture(GL_TEXTURE0 + OpenGLTexture::TEXTURE_UNIT_SHADOW_DIRECTIONAL));
        GLCALL(glBindTexture(GL_TEXTURE_2D, mShadowMap));
    }
}