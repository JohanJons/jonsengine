#pragma once

#include "include/Core/Types.h"

#include "GL/glew.h"
#include <array>

namespace JonsEngine
{
    class Logger;

    /* AccumulationBuffer definition */
    /* Accumulates color output from the light shaders and creates the resulting image */
    class AccumulationBuffer
    {
    public:
        AccumulationBuffer(Logger& logger, const uint32_t windowWidth, const uint32_t windowHeight);
        ~AccumulationBuffer();

         void BindNullForDrawing();
         void BindForDrawing();
         void BindForReading();


    private:
        Logger& mLogger;

        GLuint mFinalTexture;
        GLuint mFramebuffer;
        GLuint mDepthBuffer;
        GLuint mTextureSampler;
    };
}