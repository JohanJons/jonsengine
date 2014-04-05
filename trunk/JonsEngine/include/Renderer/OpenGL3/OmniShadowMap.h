#pragma once

#include "include/Core/Types.h"

#include "GL/glew.h"

namespace JonsEngine
{
    class Logger;

    struct OmniShadowmap
    {
        OmniShadowmap(Logger& logger, const uint32_t size);
        ~OmniShadowmap();

        void BindShadowmap();


        Logger& mLogger;

        GLuint mShadowmap;
        GLuint mTextureSampler;
        GLuint mFramebuffer;
    };
}