#include "include/Video/OpenGL/UniformBuffer.h"

namespace JonsEngine
{
    UniformBuffer::UniformBuffer(const std::string& name) : mName(name)
    {
        glGenBuffers(1, &mBuffer);
    }
        
    UniformBuffer::~UniformBuffer()
    {
        glDeleteBuffers(1, &mBuffer);
    }
}