#pragma once

#include "GL/glew.h"
#include <string>
#include <vector>

namespace JonsEngine
{
    class Logger;
    class Shader;
    class UniformBuffer;

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
        Shader* GetShader(const std::string& name);
        void ClearShaders();

        bool LinkProgram();
        void UseProgram(bool use);

        void BindAttribLocation(GLuint index, const std::string& name);
        void UseUniform(UniformBuffer& buffer, bool use);

        const std::string& GetName() const;
        bool IsLinked() const;


    private:
        Logger& mLogger;
        const std::string mName;
        GLuint mProgramHandle;
        GLuint mUniformBuffer;
        std::vector<Shader*> mAddedShaders;
    };
}