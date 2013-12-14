#pragma once

#include "include/Renderer/OpenGL3/OpenGLMesh.h"
#include "include/Core/Types.h"

#include "GL/glew.h"

namespace JonsEngine
{
    class Logger;

    class ShadingGeometry
    {
    public:
        ShadingGeometry(Logger& logger);
        ~ShadingGeometry();

        void DrawRectangle2D();
        void DrawSphere();


    private:
        OpenGLMesh mRectangleMesh;
        OpenGLMesh mSphereMesh;
        GLuint mRectangleVAO;
        GLuint mSphereVAO;
    };
}