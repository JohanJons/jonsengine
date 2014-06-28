#pragma once

#include "include/Core/Types.h"

#include "GL/glew.h"

namespace JonsEngine
{
    class Logger;

    class DirectionalShadowmap
    {
    public:
        DirectionalShadowmap(Logger& logger, const uint32_t textureSize, const uint32_t numCascades);
        ~DirectionalShadowmap();

        void BindForDrawing();
        void BindForReading();
        void BindShadowmapCascade(const uint32_t cascadeIndex);


    private:
        Logger& mLogger;

        GLuint mShadowmap;
        GLuint mTextureSampler;
        GLuint mFramebuffer;
    };
}