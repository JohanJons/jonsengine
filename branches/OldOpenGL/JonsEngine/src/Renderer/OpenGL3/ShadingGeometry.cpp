#pragma once

#include "include/Renderer/OpenGL3/ShadingGeometry.h"
#include "include/Renderer/OpenGL3/OpenGLUtils.h"

namespace JonsEngine
{
    ShadingGeometry::ShadingGeometry(Logger& logger) : mSphereMesh(CreateSphereMesh(1.0f, 12, 24, logger)), mRectangleMesh(CreateRectangleMesh (2.0f, 2.0f, 0.0f, logger)), mSphereVAO(SetupVAO(mSphereMesh)), mRectangleVAO(SetupVAO(mRectangleMesh))
    {
    }

    ShadingGeometry::~ShadingGeometry()
    {
        glDeleteVertexArrays(1, &mSphereVAO);
        glDeleteVertexArrays(1, &mRectangleVAO);
    }


    void ShadingGeometry::DrawRectangle()
    {
        GLCALL(glBindVertexArray(mRectangleVAO));
        GLCALL(glDrawElements(GL_TRIANGLES, mRectangleMesh.mIndices, GL_UNSIGNED_INT, 0));
        GLCALL(glBindVertexArray(0))
    }

    void ShadingGeometry::DrawSphere()
    {
        GLCALL(glBindVertexArray(mSphereVAO));
        GLCALL(glDrawElements(GL_TRIANGLES, mSphereMesh.mIndices, GL_UNSIGNED_INT, 0));
        GLCALL(glBindVertexArray(0))
    }
}