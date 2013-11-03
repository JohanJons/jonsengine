#pragma once

#include "include/Renderer/OpenGL3/OpenGLMesh.h"
#include "include/Renderer/OpenGL3/OpenGLTexture.h"
#include "include/Renderer/OpenGL3/ShaderProgram.h"
#include "include/Renderer/OpenGL3/UniformBuffer.hpp"
#include "include/Renderer/RenderCommands.h"
#include "include/Core/Types.h"
#include "include/Core/Memory/HeapAllocator.h"

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace JonsEngine
{
    class Logger;
    class OpenGLRenderer;
    struct EngineSettings;

    /* OpenGLRendererPtr definition */
    typedef std::unique_ptr<OpenGLRenderer, std::function<void(OpenGLRenderer*)>> ManagedOpenGLRenderer;
    typedef ManagedOpenGLRenderer& OpenGLRendererPtr;

    /* OpenGLRenderer definition */
    class OpenGLRenderer
    {
    public:
        OpenGLRenderer(const EngineSettings& engineSettings, IMemoryAllocatorPtr memoryAllocator);
        OpenGLRenderer(OpenGLRenderer& renderer, IMemoryAllocatorPtr memoryAllocator);
        OpenGLRenderer(const float anisotropy, IMemoryAllocatorPtr memoryAllocator);
        ~OpenGLRenderer();

        MeshID CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData);
        TextureID CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, TextureFormat textureFormat);

        void DrawRenderables(const RenderQueue& renderQueue, const RenderableLighting& lighting);

        float GetMaxAnisotropicFiltering() const;
        float GetCurrentAnisotropicFiltering() const;
        bool SetAnisotropicFiltering(const float newAnisoLevel);

        float GetZNear() const;
        float GetZFar() const;


    private:
        typedef GLuint VAO;
        typedef std::pair<OpenGLMeshPtr, VAO> OpenGLMeshPair;

        struct Transform
        {
            Mat4 mWVPMatrix;
            Mat4 mWorldMatrix;
            float mTextureTilingFactor;

            Transform(const Mat4& WVPMatrix, const Mat4& WorldMatrix, const float textureTilingFactor) : mWVPMatrix(WVPMatrix), mWorldMatrix(WorldMatrix), mTextureTilingFactor(textureTilingFactor)
            {
            }
        };

        struct Material
        {
            Vec4 mDiffuseColor;
            Vec4 mAmbientColor;
            Vec4 mSpecularColor;
            Vec4 mEmissiveColor;
            uint32_t mHasDiffuseTexture;
            uint32_t mHasNormalTexture;
            uint32_t mLightingEnabled;
            float mSpecularFactor;

            Material(const Vec4& diffuseColor, const Vec4& ambientColor, const Vec4& specularColor, const Vec4& emissiveColor, const bool hasDiffuseTexture, const bool hasNormalTexture, const bool lightingEnabled, const float specularFactor) 
                : 
                mDiffuseColor(diffuseColor), mAmbientColor(ambientColor), mSpecularColor(specularColor),  mEmissiveColor(emissiveColor), mHasDiffuseTexture(hasDiffuseTexture), mHasNormalTexture(hasNormalTexture), mLightingEnabled(lightingEnabled), mSpecularFactor(specularFactor)
            {
            }
        };

        VAO SetupVAO(OpenGLMeshPtr mesh);

        IMemoryAllocatorPtr mMemoryAllocator;
        Logger& mLogger;

        std::vector<OpenGLMeshPair> mMeshes;
        std::vector<OpenGLTexturePtr> mTextures;
        ShaderProgram mDefaultProgram;
        UniformBuffer<OpenGLRenderer::Transform> mUniBufferTransform;
        UniformBuffer<OpenGLRenderer::Material> mUniBufferMaterial;
        UniformBuffer<RenderableLighting> mUniBufferLightingInfo;
        GLuint mTextureSampler;
        float mCurrentAnisotropy;
    };


    /* OpenGLRenderBackend inlines */
    inline float OpenGLRenderer::GetZNear() const                                          { return 0.5f; }
    inline float OpenGLRenderer::GetZFar() const                                           { return 1000.0f; }
}