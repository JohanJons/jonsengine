#pragma once

#include "include/Renderer/OpenGL3/OpenGLMesh.h"
#include "include/Renderer/OpenGL3/OpenGLTexture.h"
#include "include/Renderer/OpenGL3/ShaderProgram.hpp"
#include "include/Renderer/OpenGL3/GBuffer.h"
#include "include/Renderer/OpenGL3/UniformBuffer.hpp"
#include "include/Renderer/OpenGL3/ShadingGeometry.h"
#include "include/Renderer/OpenGL3/DirectionalShadowmap.h"
#include "include/Renderer/OpenGL3/OmnidirectionalShadowmap.h"
#include "include/Renderer/OpenGL3/UniformBufferDefinitions.hpp"
#include "include/Renderer/RenderCommands.h"
#include "include/Core/Types.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/DebugOptions.h"

#include "GL/glew.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace JonsEngine
{
    struct EngineSettings;
    class Logger;
    class OpenGLRenderer;

    /* OpenGLRendererPtr definition */
    typedef std::unique_ptr<OpenGLRenderer, std::function<void(OpenGLRenderer*)>> ManagedOpenGLRenderer;
    typedef ManagedOpenGLRenderer& OpenGLRendererPtr;

    /* OpenGLRenderer definition */
    class OpenGLRenderer
    {
    public:
        typedef std::pair<OpenGLMeshPtr, GLuint> OpenGLMeshPair;


        OpenGLRenderer(const EngineSettings& engineSettings, IMemoryAllocatorPtr memoryAllocator);
        OpenGLRenderer(const std::vector<OpenGLMeshPtr>& meshes, const std::vector<OpenGLTexturePtr>& textures, const uint32_t windowWidth, const uint32_t windowHeight, const uint32_t shadowMapResolution, const float anisotropy, IMemoryAllocatorPtr memoryAllocator);
        OpenGLRenderer(const uint32_t windowWidth, const uint32_t windowHeight, const uint32_t shadowMapResolution, const float anisotropy, IMemoryAllocatorPtr memoryAllocator);
        ~OpenGLRenderer();

        MeshID CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData);
        TextureID CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ColorFormat colorFormat);

        void Render(const RenderQueue& renderQueue, const RenderableLighting& lighting, const DebugOptions::RenderingMode debugMode, const DebugOptions::RenderingFlags debugExtra);

        float GetMaxAnisotropicFiltering() const;
        float GetCurrentAnisotropicFiltering() const;
        void SetAnisotropicFiltering(const float newAnisoLevel);

        std::vector<OpenGLMeshPtr> GetMeshes() const;
        std::vector<OpenGLTexturePtr> GetTextures() const;

        float GetZNear() const;
        float GetZFar() const;
        uint32_t GetShadowmapResolution() const;


    private:
        void GeometryStage(const RenderQueue& renderQueue, const RenderableLighting& lighting, const bool debugLights);
        void ShadingStage(const RenderQueue& renderQueue, const RenderableLighting& lighting, const bool debugShadowmapSplits);
        void AmbientLightPass(const Vec4& ambientLight, const Vec4& gamma, const Vec2& screenSize);
        void GeometryDepthPass(const RenderQueue& renderQueue, const Mat4& lightVP);
        void PointLightShadowPass(const RenderQueue& renderQueue, const Mat4& lightProjMatrix, const Mat4& lightViewMatrix);
        void PointLightStencilPass(const RenderableLighting::PointLight& pointLight);
        void PointLightLightingPass(const RenderableLighting::PointLight& pointLight, const Vec4& gamma, const Vec2& screenSize);
        void DirLightShadowPass(const RenderQueue& renderQueue, const Mat4& lightVP, const uint8_t cascadeIndex);
        void DirLightLightingPass(const RenderableLighting::DirectionalLight& dirLight, const std::array<Mat4, 4>& lightMatrices, const Mat4& cameraViewMatrix, const std::array<float, 4>& splitDistances, const Vec4& gamma, const Vec2& screenSize, const bool debugShadowmapSplits);
        void RenderToScreen(const DebugOptions::RenderingMode debugOptions, const Vec2& screenSize);

        IMemoryAllocatorPtr mMemoryAllocator;
        Logger& mLogger;

        ShaderProgram<UnifGeometry> mGeometryProgram;
        ShaderProgram<UnifNull> mNullProgram;
        ShaderProgram<UnifAmbientLight> mAmbientProgram;
        ShaderProgram<UnifPointLight> mPointLightProgram;
        ShaderProgram<UnifDirLight> mDirLightProgram;
        ShaderProgram<UnifDirLight> mDirLightDebugProgram;
        ShaderProgram<UnifDepth> mDepthProgram;
        GBuffer mGBuffer;
        GLuint mTextureSampler;
        float mCurrentAnisotropy;
        uint32_t mWindowWidth;
        uint32_t mWindowHeight;
        uint32_t mShadowmapResolution;

        std::vector<OpenGLMeshPair> mMeshes;
        std::vector<OpenGLTexturePtr> mTextures;
        ShadingGeometry mShadingGeometry;
        DirectionalShadowmap mDirectionalShadowmap;
        OmnidirectionalShadowmap mOmnidirectionalShadowmap;
    };
}