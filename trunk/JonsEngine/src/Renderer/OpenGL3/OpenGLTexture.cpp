#include "include/Renderer/OpenGL3/OpenGLTexture.h"

namespace JonsEngine
{
    OpenGLTexture::OpenGLTexture(const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ITexture::TextureFormat textureFormat)
    {
        glGenTextures(1, &mTexture);
        glBindTexture(GL_TEXTURE_2D, mTexture);

        GLint glTextureFormat = ParseTextureFormat(textureFormat);
        glTexImage2D(GL_TEXTURE_2D, 0, glTextureFormat, textureWidth, textureHeight, 0, glTextureFormat, GL_UNSIGNED_BYTE, &textureData[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    }
        
    OpenGLTexture::~OpenGLTexture()
    {
        glDeleteBuffers(1, &mTexture);
    }


    GLint OpenGLTexture::ParseTextureFormat(ITexture::TextureFormat textureFormat)
    {
        switch (textureFormat)
        {
            case ITexture::RGB:
                return GL_RGB;

            case ITexture::RGBA:
                return GL_RGBA;

            default:
                return GL_RED;
        }
    }
}