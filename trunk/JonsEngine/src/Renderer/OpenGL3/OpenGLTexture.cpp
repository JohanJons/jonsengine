#include "include/Renderer/OpenGL3/OpenGLTexture.h"

#include "include/Renderer/OpenGL3/OpenGLUtils.hpp"
#include "include/Core/Logging/Logger.h"

namespace JonsEngine
{
    TextureID gNextTextureID = 1;

    OpenGLTexture::OpenGLTexture(const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, TextureFormat textureFormat, TextureType textureType, Logger& logger) : mLogger(logger), mTextureID(gNextTextureID++), mTextureType(textureType)
    {
        glGenTextures(1, &mTexture);
        CHECK_GL_ERROR(mLogger);

        glBindTexture(GL_TEXTURE_2D, mTexture);
        CHECK_GL_ERROR(mLogger);

        GLint glTextureFormat = textureFormat == TextureFormat::TEXTURE_FORMAT_RGB ? GL_RGB : TextureFormat::TEXTURE_FORMAT_RGBA ? GL_RGBA : GL_RED;
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
}