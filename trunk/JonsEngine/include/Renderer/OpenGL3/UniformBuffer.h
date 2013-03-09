#pragma once

#include "include/Core/EngineDefs.h"

#include "GL/glew.h"
#include <vector>
#include <string>
#include <algorithm>

namespace JonsEngine
{
    /* UniformBuffer definition */
    class UniformBuffer
    {
    public:
        friend class ShaderProgram;

        UniformBuffer(const std::string& name);
        ~UniformBuffer();

        template<typename T>
        void SetData(const T& data);


    private:
        const std::string mName;
        GLuint mBindingIndex;
        GLuint mBuffer;
        size_t mBufferSize;
    };


    /* UniformBuffer inlines */
    template<typename T>
    inline void UniformBuffer::SetData(const T& data)
    {
        std::vector<float> buffer;
        data.CopyUniformData(buffer);

        mBufferSize = buffer.size() * sizeof(float);

        glBindBuffer(GL_UNIFORM_BUFFER, mBuffer);
        glBufferData(GL_UNIFORM_BUFFER, mBufferSize, &buffer[0], GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferRange(GL_UNIFORM_BUFFER, mBindingIndex, mBuffer, 0, mBufferSize);
    }
}