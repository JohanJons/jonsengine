#pragma once

#include "include/Renderer/IRenderer.h"
#include "include/Renderer/IMaterial.h"
#include "include/Renderer/IVertexBuffer.h"
#include "include/Renderer/Renderable.h"
#include "include/Renderer/RenderableLighting.h"
#include "include/Renderer/OpenGL3/ShaderProgram.h"
#include "include/Renderer/OpenGL3/UniformBuffer.hpp"

#include <string>
#include <vector>

namespace JonsEngine
{
    struct EngineSettings;
    class Logger;


    /* OpenGLRenderer definition */
    class OpenGLRenderer : public IRenderer
    {
    public:
        OpenGLRenderer(const EngineSettings& engineSettings);
        ~OpenGLRenderer();

        VertexBufferPtr CreateVertexBuffer(const std::vector<Vec3>& vertexData, const std::vector<Vec3>& normalData, const std::vector<Vec2>& texCoords, const std::vector<uint32_t>& indexData);
        MaterialPtr CreateMaterial();

        void DrawRenderables(const RenderQueue& renderQueue, const RenderableLighting& lighting);
        
        RenderBackendType GetRenderBackendType() const;


    private:
        ShaderProgram SetupShaderProgram(const std::string& programName);

        ShaderProgram mDefaultProgram;
        UniformBuffer<Renderable::Transform> mUniBufferTransform;
        UniformBuffer<Renderable::Material> mUniBufferMaterial;
        UniformBuffer<RenderableLighting::LightingInfo> mUniBufferLightingInfo;
        UniformBuffer<RenderableLighting::Lights> mUniBufferLights;

        Logger& mLogger;
    };


    /* OpenGLRenderBackend inlines */
    inline IRenderer::RenderBackendType OpenGLRenderer::GetRenderBackendType() const       { return OPENGL; }
}