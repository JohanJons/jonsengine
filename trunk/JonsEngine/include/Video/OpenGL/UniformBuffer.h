#pragma once

#include "include/Core/EngineDefs.h"

#include "GL/glew.h"
#include <string>

namespace JonsEngine
{
    /* UniformBuffer definition */
    class UniformBuffer
    {
    public:
        UniformBuffer(const std::string& name);
        ~UniformBuffer();

        template<typename T>
        void SetData(const T& data);


    private:
        const std::string mName;
        GLuint mBuffer;
    };


    /* UniformBuffer inlines */
    template<typename T>
    inline void UniformBuffer::SetData(const T& data)
    {
        uint8_t buffer[sizeof(T)];
        data.CopyUniformData(buffer);

        glBindBuffer(GL_UNIFORM_BUFFER, mBuffer);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(T), buffer, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}