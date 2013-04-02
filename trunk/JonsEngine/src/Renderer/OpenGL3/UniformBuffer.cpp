#include "include/Renderer/OpenGL3/UniformBuffer.h"

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

    void UniformBuffer::SetData(const std::vector<float>& buffer)
    {
        mBufferSize = buffer.size() * sizeof(float);

        glBindBuffer(GL_UNIFORM_BUFFER, mBuffer);
        glBufferData(GL_UNIFORM_BUFFER, mBufferSize, &buffer[0], GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferRange(GL_UNIFORM_BUFFER, mBindingIndex, mBuffer, 0, mBufferSize);
    }
}