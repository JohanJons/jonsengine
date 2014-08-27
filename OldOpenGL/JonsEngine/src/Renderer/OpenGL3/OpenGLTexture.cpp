#include "include/Renderer/OpenGL3/OpenGLTexture.h"

#include "include/Renderer/OpenGL3/OpenGLUtils.h"
#include "include/Core/Logging/Logger.h"

namespace JonsEngine
{
    TextureID gNextTextureID = 1;

    OpenGLTexture::OpenGLTexture(const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ColorFormat colorFormat, TextureType textureType, Logger& logger) : mLogger(logger), mTextureID(gNextTextureID++), mTextureType(textureType)
    {
        GLCALL(glGenTextures(1, &mTexture));

        GLCALL(glBindTexture(GL_TEXTURE_2D, mTexture));

        GLint format = (colorFormat == ColorFormat::COLOR_FORMAT_RGB ? GL_RGB : colorFormat == ColorFormat::COLOR_FORMAT_RGBA ? GL_RGBA : GL_RED);
        GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, format, textureWidth, textureHeight, 0, format, GL_UNSIGNED_BYTE, &textureData[0]));
        
        GLCALL(glGenerateMipmap(GL_TEXTURE_2D));
       
        GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
    }
        
    OpenGLTexture::~OpenGLTexture()
    {
        GLCALL(glDeleteTextures(1, &mTexture));
    }
}