#pragma once

#include "include/Renderer/ITexture.h"
#include "include/Renderer/OpenGL3/Shader.h"

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
        ShaderProgram(const std::string& name, const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
        ~ShaderProgram();

        void UseProgram(bool use);
        void BindAttribLocation(GLuint index, const std::string& name);
        void BindTextureUnit(ITexture::TextureType textureType, GLuint textureUnit);

        const std::string& GetName() const;
        GLuint GetHandle() const;


    private:
        Logger& mLogger;
        const std::string mName;
        GLuint mProgramHandle;
        Shader mVertexShader;
        Shader mFragmentShader;
    };
}