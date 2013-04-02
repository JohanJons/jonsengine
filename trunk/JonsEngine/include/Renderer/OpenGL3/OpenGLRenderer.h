#pragma once

#include "include/Renderer/IRenderer.h"
#include "include/Renderer/RenderItem.h"
#include "include/Renderer/OpenGL3/Shaders.h"
#include "include/Renderer/OpenGL3/ShaderProgram.h"
#include "include/Renderer/OpenGL3/UniformBuffer.h"
#include "include/Scene/Mesh.h"

#include <string>
#include <vector>

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

        VertexBufferPtr CreateVertexBuffer(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<uint32_t>& indexData);
 
        void BeginRendering();
        void DrawRenderables(const std::vector<RenderItem>& renderQueue);
        void EndRendering();
        
        RenderBackendType GetRenderBackendType() const;


    private:
        void SetupShaders();

        std::vector<RenderItem> mRenderQueue;
        ShaderProgram mDefaultProgram;
        UniformBuffer mUniBufferTransform;
        UniformBuffer mUniBufferLight;

        Logger& mLogger;
        IMemoryAllocator& mMemoryAllocator;
    };


    /* OpenGLRenderBackend inlines */
    inline IRenderer::RenderBackendType OpenGLRenderer::GetRenderBackendType() const       { return OPENGL; }
}