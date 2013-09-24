#include "include/Renderer/OpenGL3/OpenGLTexture.h"

#include "include/Renderer/OpenGL3/OpenGLUtils.hpp"
#include "include/Core/Logging/Logger.h"


namespace JonsEngine
{
    OpenGLTexture::OpenGLTexture(const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ITexture::TextureFormat textureFormat) : mLogger(Logger::GetRendererLogger())
    {
        glGenTextures(1, &mTexture);
        CHECK_GL_ERROR(mLogger);
        glBindTexture(GL_TEXTURE_2D, mTexture);
        CHECK_GL_ERROR(mLogger);
        GLint glTextureFormat = ParseTextureFormat(textureFormat);
        glTexImage2D(GL_TEXTURE_2D, 0, glTextureFormat, textureWidth, textureHeight, 0, glTextureFormat, GL_UNSIGNED_BYTE, &textureData[0]);
        CHECK_GL_ERROR(mLogger);
        glGenerateMipmap(GL_TEXTURE_2D);
        CHECK_GL_ERROR(mLogger);
        glBindTexture(GL_TEXTURE_2D, 0);
        CHECK_GL_ERROR(mLogger);
    }
        
    OpenGLTexture::~OpenGLTexture()
    {
        glDeleteBuffers(1, &mTexture);
        CHECK_GL_ERROR(mLogger);
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