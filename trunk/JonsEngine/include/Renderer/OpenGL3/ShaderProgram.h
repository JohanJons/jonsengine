#pragma once

#include "GL/glew.h"
#include <string>
#include <vector>

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
        Shader* GetShader(const std::string& name);
        void ClearShaders();

        bool LinkProgram();
        void UseProgram(bool use);
        void BindAttribLocation(GLuint index, const std::string& name);

        const std::string& GetName() const;
        GLuint GetHandle() const;
        bool IsLinked() const;


    private:
        Logger& mLogger;
        const std::string mName;
        GLuint mProgramHandle;
        std::vector<Shader*> mAddedShaders;
    };
}