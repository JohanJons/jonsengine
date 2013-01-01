#pragma once

#include "include/Renderer/IRenderer.h"
#include "include/Renderer/OpenGL3/Shaders.h"
#include "include/Renderer/OpenGL3/ShaderProgram.h"
#include "include/Renderer/OpenGL3/UniformBuffer.h"
#include "include/Scene/Mesh.h"

#include <string>

namespace JonsEngine
{
    struct EngineSettings;
    class Logger;
    class IMemoryAllocator;


    /* OpenGLRenderer definition */
    class OpenGLRenderer : public IRenderer
    {
    public:
        OpenGLRenderer(const EngineSettings& engineSettings);
        ~OpenGLRenderer();

        VertexBufferPtr CreateVertexBuffer(const vector<float>& vertexData, const vector<uint16_t>& indexData);
        VertexBufferPtr CreateVertexBuffer(const float vertexData[], const size_t vertexDataSize, const uint16_t indexData[], const size_t indexDataSize);

        void BeginRendering();
        void RenderMesh(MeshPtr mesh);
        void EndRendering();
        
        RenderBackendType GetRenderBackendType() const;


    private:
        void SetupShaders();

        Logger& mLogger;
        IMemoryAllocator& mMemoryAllocator;
        ShaderProgram mDefaultProgram;
        UniformBuffer mUniBuffer;
    };


    /* OpenGLRenderBackend inlines */
    inline IRenderer::RenderBackendType OpenGLRenderer::GetRenderBackendType() const       { return OPENGL; }
}