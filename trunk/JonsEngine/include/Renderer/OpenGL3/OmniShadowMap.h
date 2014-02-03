#pragma once

#include "include/Core/Types.h"

#include "GL/glew.h"

namespace JonsEngine
{
    class Logger;

    struct OmniShadowMap
    {
        OmniShadowMap(Logger& logger, const uint32_t size);
        ~OmniShadowMap();

        void BindShadowMap();


        Logger& mLogger;

        GLuint mShadowMap;
        GLuint mTextureSampler;
        GLuint mFramebuffer;
    };
}