#pragma once

#include "include/Renderer/OpenGL3/OmniShadowmap.h"

#include "include/Renderer/OpenGL3/OpenGLTexture.h"
#include "include/Renderer/OpenGL3/OpenGLUtils.h"
#include "include/Core/Logging/Logger.h"

namespace JonsEngine
{
    const uint32_t CUBEMAP_NUM_FACES = 6;   // posX => negX => posY => negY => posZ => negZ
    const Vec3 CUBEMAP_DIRECTION_VECTORS[CUBEMAP_NUM_FACES] = { Vec3(1.0f, 0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f),
                                                                Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f) };
    const Vec3 CUBEMAP_UP_VECTORS[CUBEMAP_NUM_FACES] = { Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f),
                                                         Vec3(0.0f, 0.0f, -1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f) };


    OmniShadowmap::OmniShadowmap(Logger& logger, const uint32_t size) : mLogger(logger)
    {
        GLCALL(glGenFramebuffers(1, &mFramebuffer));
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer));

        GLCALL(glGenTextures(1, &mShadowmap));
        GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, mShadowmap));
        GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
        for (uint32_t i = 0; i < CUBEMAP_NUM_FACES; i++)
            GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0));
        GLCALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mShadowmap, 0));
        GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));

        GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        {
            JONS_LOG_ERROR(mLogger, "Framebuffer incomplete");
            throw std::runtime_error("Framebuffer incomplete");
        }
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

        GLCALL(glGenSamplers(1, &mTextureSampler));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
        GLCALL(glBindSampler(OpenGLTexture::TEXTURE_UNIT_SHADOW_OMNI, mTextureSampler));
    }

    OmniShadowmap::~OmniShadowmap()
    {
        GLCALL(glDeleteTextures(1, &mShadowmap));
        GLCALL(glDeleteSamplers(1, &mTextureSampler));
        GLCALL(glDeleteFramebuffers(1, &mFramebuffer));
    }


    void OmniShadowmap::BindShadowmap()
    {
    }
}