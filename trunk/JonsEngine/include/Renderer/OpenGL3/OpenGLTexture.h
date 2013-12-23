#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Core/Types.h"

#include "GL/glew.h"
#include <vector>
#include <memory>

namespace JonsEngine
{
    class Logger;
    struct OpenGLTexture;

    /*
     * OpenGLTexturePtr definition
     */
    typedef std::shared_ptr<OpenGLTexture> OpenGLTexturePtr;

    /*
     * OpenGLTexture definition
     */
    struct OpenGLTexture
    {
        enum TextureUnit
        {
            TEXTURE_UNIT_GEOMETRY_DIFFUSE = 0,
            TEXTURE_UNIT_GEOMETRY_NORMAL,
            TEXTURE_UNIT_GBUFFER_POSITION,
            TEXTURE_UNIT_GBUFFER_NORMAL,
            TEXTURE_UNIT_GBUFFER_DIFFUSE
        };

        OpenGLTexture(const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ColorFormat colorFormat, TextureType textureType, Logger& logger);
        ~OpenGLTexture();


        Logger& mLogger;
        TextureID mTextureID;

        TextureType mTextureType;
        GLuint mTexture;
    };
}