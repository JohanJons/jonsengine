#pragma once

#include "include/Core/Containers/Vector.h"

#include "GL/glew.h"

#include <string>

namespace JonsEngine
{
    class Logger;
    class Shader;

    /*
     * C++ wrapper around GLSL shaderprograms
     */

    /* ShaderProgram definition */
    class ShaderProgram
    {
    public:
        ShaderProgram(const std::string& name);
        ~ShaderProgram();

        void AddShader(Shader* shader);
        void RemoveShader(Shader* shader);
        void ClearShaders();

        bool LinkProgram();
        void UseProgram(bool use);

        template <typename T>
        void SetUniformBuffer(const T& uniBuffer, const std::string& name);

        void BindAttribLocation(GLuint index, const std::string& name);

        const std::string& GetName() const;
        bool IsLinked() const;


    private:
        Logger& mLogger;
        const std::string mName;
        GLuint mProgramHandle;
        GLuint mUniformBuffer;
        Vector<Shader*> mAddedShaders;
    };




    /* ShaderProgram inlines */
    template <typename T>
    void ShaderProgram::SetUniformBuffer(const T& uniBuffer, const std::string& name)
    {
        uint8_t buffer[sizeof(T)];

        glBindBuffer(GL_UNIFORM_BUFFER, mUniformBuffer);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(uniBuffer), &uniBuffer, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glUniformBlockBinding(mProgramHandle, glGetUniformBlockIndex(mProgramHandle, name.c_str()), 0);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, mUniformBuffer, 0, sizeof(uniBuffer));
    }
}