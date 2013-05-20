#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Renderer/ITexture.h"

#include "GL/glew.h"
#include <vector>

namespace JonsEngine
{
    /*
     * OpenGLTexture definition
     */
    class OpenGLTexture : public ITexture
    {
    public:
        enum TextureUnit
        {
            TEXTURE_UNIT_DIFFUSE = 0,
            TEXTURE_UNIT_UNKNOWN
        };

        OpenGLTexture(const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ITexture::TextureFormat textureFormat);
        ~OpenGLTexture();

        GLuint mTexture;

    private:
        GLint ParseTextureFormat(ITexture::TextureFormat textureFormat);
    };
}