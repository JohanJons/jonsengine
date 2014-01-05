#pragma once

#include "include/Core/Types.h"

#include "GL/glew.h"

namespace JonsEngine
{
    class Logger;

    struct DirectionalShadowMap
    {
        DirectionalShadowMap(Logger& logger, const uint32_t windowWidth, const uint32_t windowHeight);
        ~DirectionalShadowMap();

        void BindShadowMap();


        Logger& mLogger;

        GLuint mShadowMap;
        GLuint mTextureSampler;
        GLuint mFramebuffer;
    };
}