#pragma once

#include "include/Core/Types.h"

#include "GL/glew.h"

namespace JonsEngine
{
    class Logger;

    class OmnidirectionalShadowmap
    {
    public:
        OmnidirectionalShadowmap(Logger& logger, const uint32_t size, const uint32_t numFaces);
        ~OmnidirectionalShadowmap();

        void BindForDrawing();
        void BindForReading();
        void BindShadowmapFace(const uint32_t face);


    private:
        Logger& mLogger;

        GLuint mShadowmap;
        GLuint mTextureSampler;
        GLuint mFramebuffer;
    };
}