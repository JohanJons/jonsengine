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

        void SetData(const std::vector<float>& buffer);


    private:
        const std::string mName;
        GLuint mBindingIndex;
        GLuint mBuffer;
        size_t mBufferSize;
    };
}