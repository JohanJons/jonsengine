#pragma once

#include "include/Renderer/OpenGL3/DirectionalShadowmap.h"

#include "include/Renderer/OpenGL3/OpenGLTexture.h"
#include "include/Renderer/OpenGL3/OpenGLUtils.h"
#include "include/Core/Logging/Logger.h"

namespace JonsEngine
{
    DirectionalShadowmap::DirectionalShadowmap(Logger& logger, const uint32_t textureSize, const uint32_t numCascades) : mLogger(logger)
    {
        GLCALL(glGenFramebuffers(1, &mFramebuffer));

        // shadow map texture
        GLCALL(glGenTextures(1, &mShadowmap));
        GLCALL(glBindTexture(GL_TEXTURE_2D_ARRAY, mShadowmap));
        GLCALL(glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32, textureSize, textureSize, numCascades, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0));
        GLCALL(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));

        // color attachment texture sampler
        GLCALL(glGenSamplers(1, &mTextureSampler));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
        GLCALL(glBindSampler(OpenGLTexture::TEXTURE_UNIT_SHADOW_DIRECTIONAL, mTextureSampler));
    }

    DirectionalShadowmap::~DirectionalShadowmap()
    {
        GLCALL(glDeleteTextures(1, &mShadowmap));
        GLCALL(glDeleteSamplers(1, &mTextureSampler));
        GLCALL(glDeleteFramebuffers(1, &mFramebuffer));
    }


    void DirectionalShadowmap::BindForDrawing()
    {
        GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFramebuffer));
        GLCALL(glDrawBuffer(GL_NONE));
    }

    void DirectionalShadowmap::BindForReading()
    {
        GLCALL(glActiveTexture(GL_TEXTURE0 + OpenGLTexture::TEXTURE_UNIT_SHADOW_DIRECTIONAL));
        GLCALL(glBindTexture(GL_TEXTURE_2D_ARRAY, mShadowmap));
    }

    void DirectionalShadowmap::BindShadowmapCascade(const uint32_t cascadeIndex)
    {
        GLCALL(glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mShadowmap, 0, cascadeIndex));
    }
}