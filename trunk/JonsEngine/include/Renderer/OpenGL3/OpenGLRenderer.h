#pragma once

#include "include/Renderer/OpenGL3/OpenGLMesh.h"
#include "include/Renderer/OpenGL3/OpenGLTexture.h"
#include "include/Renderer/OpenGL3/ShaderProgram.h"
#include "include/Renderer/OpenGL3/GBuffer.h"
#include "include/Renderer/OpenGL3/UniformBuffer.hpp"
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
        OpenGLRenderer(const EngineSettings& engineSettings, IMemoryAllocatorPtr memoryAllocator);
        OpenGLRenderer(OpenGLRenderer& renderer, IMemoryAllocatorPtr memoryAllocator);
        OpenGLRenderer(const uint32_t windowWidth, const uint32_t windowHeight, const float anisotropy, IMemoryAllocatorPtr memoryAllocator);
        ~OpenGLRenderer();

        MeshID CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData);
        TextureID CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ColorFormat colorFormat);

        void DrawRenderables(const RenderQueue& renderQueue, const RenderableLighting& lighting, const DebugOptions::RenderingMode debugOptions);

        float GetMaxAnisotropicFiltering() const;
        float GetCurrentAnisotropicFiltering() const;
        bool SetAnisotropicFiltering(const float newAnisoLevel);

        float GetZNear() const;
        float GetZFar() const;


    private:
        typedef std::pair<OpenGLMeshPtr, GLuint> OpenGLMeshPair;

        struct UnifTransform
        {
            Mat4 mWVPMatrix;
            Mat4 mWorldMatrix;
            float mTextureTilingFactor;

            UnifTransform(const Mat4& WVPMatrix, const Mat4& WorldMatrix, const float textureTilingFactor) : mWVPMatrix(WVPMatrix), mWorldMatrix(WorldMatrix), mTextureTilingFactor(textureTilingFactor)
            {
            }
        };

        /*struct UnifLight
        {
            Mat4  mViewMatrix;
            Vec4  mGamma;
            Vec3  mViewDirection;
            int   mNumLights;
            RenderableLighting::Light mLight;

            UnifLight(const Mat4& viewMatrix, const Vec4& gamma, const Vec3& viewDir, const int keke, const RenderableLighting::Light& light) : mViewMatrix(viewMatrix), mGamma(gamma), mViewDirection(viewDir), mNumLights(keke), mLight(light)
            {
            }
        };*/

        struct UnifMaterial
        {
            Vec4 mDiffuseColor;
            Vec4 mAmbientColor;
            Vec4 mSpecularColor;
            Vec4 mEmissiveColor;
            uint32_t mHasDiffuseTexture;
            uint32_t mHasNormalTexture;
            uint32_t mLightingEnabled;
            float mSpecularFactor;

            UnifMaterial(const Vec4& diffuseColor, const Vec4& ambientColor, const Vec4& specularColor, const Vec4& emissiveColor, const bool hasDiffuseTexture, const bool hasNormalTexture, const bool lightingEnabled, const float specularFactor) 
                : 
                mDiffuseColor(diffuseColor), mAmbientColor(ambientColor), mSpecularColor(specularColor),  mEmissiveColor(emissiveColor), mHasDiffuseTexture(hasDiffuseTexture), mHasNormalTexture(hasNormalTexture), mLightingEnabled(lightingEnabled), mSpecularFactor(specularFactor)
            {
            }
        };

        void GeometryPass(const RenderQueue& renderQueue);
        void ShadingPass();
        void DrawGBufferToScreen(const DebugOptions::RenderingMode debugOptions);

        IMemoryAllocatorPtr mMemoryAllocator;
        Logger& mLogger;

        ShaderProgram mGeometryProgram;
        ShaderProgram mShadingProgram;
        ShaderProgram mDebugProgram;
        GBuffer mGBuffer;
        GLuint mTextureSampler;
        float mCurrentAnisotropy;
        uint32_t mWindowWidth;
        uint32_t mWindowHeight;

        std::vector<OpenGLMeshPair> mMeshes;
        std::vector<OpenGLTexturePtr> mTextures;
        UniformBuffer<OpenGLRenderer::UnifTransform> mUniBufferTransform;
        UniformBuffer<OpenGLRenderer::UnifMaterial> mUniBufferMaterial;
    };
}