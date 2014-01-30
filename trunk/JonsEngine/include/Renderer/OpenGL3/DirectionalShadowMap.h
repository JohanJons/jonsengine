#pragma once

#include "include/Core/Types.h"

#include "GL/glew.h"

namespace JonsEngine
{
    class Logger;

    struct DirectionalShadowMap
    {
        DirectionalShadowMap(Logger& logger, const uint32_t width, const uint32_t height);
        ~DirectionalShadowMap();

        void BindShadowMap();


        Logger& mLogger;

        GLuint mShadowMap;
        GLuint mTextureSampler;
        GLuint mFramebuffer;
    };
}