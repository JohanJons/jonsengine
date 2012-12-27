#include "include/Video/OpenGL3/UniformBuffer.h"

namespace JonsEngine
{
    static GLuint gNextBindingIndex = 0;

    UniformBuffer::UniformBuffer(const std::string& name) : mName(name), mBindingIndex(++gNextBindingIndex), mBufferSize(0)
    {
        glGenBuffers(1, &mBuffer);
    }
        
    UniformBuffer::~UniformBuffer()
    {
        glDeleteBuffers(1, &mBuffer);
    }
}