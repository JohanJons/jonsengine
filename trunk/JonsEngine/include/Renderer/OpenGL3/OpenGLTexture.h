#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Core/Types.h"

#include "GL/glew.h"
#include <vector>

namespace JonsEngine
{
    class Logger;

    /*
     * OpenGLTexture definition
     */
    struct OpenGLTexture
    {
        enum TextureUnit
        {
            TEXTURE_UNIT_UNKNOWN = -1,
            TEXTURE_UNIT_DIFFUSE = 0
        };

        OpenGLTexture(const TextureID textureID, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, TextureFormat textureFormat, TextureType textureType);
        ~OpenGLTexture();

        TextureID mTextureID;
        Logger& mLogger;

        TextureType mTextureType;
        GLuint mTexture;
    };
}