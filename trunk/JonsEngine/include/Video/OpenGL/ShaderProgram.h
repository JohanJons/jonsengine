#pragma once

#include "include/Core/Containers/Vector.h"

#include "GL/glew.h"

#include <string>

namespace JonsEngine
{
    class Logger;
    class Shader;

    class ShaderProgram
    {
    public:
        ShaderProgram(const std::string& name);
        ~ShaderProgram();

        void AddShader(Shader* shader);
        void RemoveShader(Shader* shader);
        void ClearShaders();

        void BindAttribLocation(GLuint index, const std::string& name);
        bool LinkProgram();
        void UnlinkProgram();
        void UseProgram(bool use);
        void SetName(const std::string& name);

        const std::string& GetName() const;
        bool IsLinked() const;


    private:
        Logger& mLogger;
        std::string mName;
        bool mIsLinked;
        GLuint mProgramHandle;
        Vector<Shader*> mAddedShaders;
    };


}