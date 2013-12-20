#pragma once

#include "include/Renderer/OpenGL3/OpenGLMesh.h"
#include "include/Renderer/OpenGL3/OpenGLTexture.h"
#include "include/Renderer/OpenGL3/ShaderProgram.h"
#include "include/Renderer/OpenGL3/GBuffer.h"
#include "include/Renderer/OpenGL3/UniformBuffer.hpp"
#include "include/Renderer/OpenGL3/ShadingGeometry.h"
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

        void DrawRenderables(const RenderQueue& renderQueue, const RenderableLighting& lighting, const DebugOptions::RenderingMode debugMode, const DebugOptions::RenderingFlags debugExtra);

        float GetMaxAnisotropicFiltering() const;
        float GetCurrentAnisotropicFiltering() const;
        void SetAnisotropicFiltering(const float newAnisoLevel);

        float GetZNear() const;
        float GetZFar() const;


    private:
        typedef std::pair<OpenGLMeshPtr, GLuint> OpenGLMeshPair;

        struct UnifGeometry
        {
            Mat4 mWVPMatrix;
            Mat4 mWorldMatrix;
            uint32_t mHasDiffuseTexture;
            uint32_t mHasNormalTexture;
            float mTextureTilingFactor;

            UnifGeometry(const Mat4& WVPMatrix, const Mat4& WorldMatrix, const uint32_t hasDiffuseTexture, const uint32_t hasNormalTexture, const float textureTilingFactor) 
                : 
                mWVPMatrix(WVPMatrix), mWorldMatrix(WorldMatrix), mHasDiffuseTexture(hasDiffuseTexture), mHasNormalTexture(hasNormalTexture), mTextureTilingFactor(textureTilingFactor)
            {
            }
        };

        struct UnifShading
        {
            enum LightType
            {
                LIGHT_TYPE_UNKNOWN = 0,
                LIGHT_TYPE_POINT,
                LIGHT_TYPE_DIRECTIONAL,
                LIGHT_TYPE_AMBIENT
            };

            Mat4 mWVPMatrix;
            Vec4 mLightColor;
            Vec4 mLightPosOrDir;
            Vec4 mGamma;
            Vec2 mScreenSize;
            uint32_t mLightType;
            float mFalloffFactor;
            float mMaxDistance;

            UnifShading(const Mat4& vwpMatrix, const Vec4& lightColor, const Vec4& lightPosOrDir, const Vec4& gamma, const Vec2& screenSize, const uint32_t lightType, const float falloffFactor, const float maxDistance) :
                mWVPMatrix(vwpMatrix), mLightColor(lightColor), mLightPosOrDir(lightPosOrDir), mGamma(gamma), mScreenSize(screenSize), mLightType(lightType), mFalloffFactor(falloffFactor), mMaxDistance(maxDistance)
            {
            }
        };

        void GeometryPass(const RenderQueue& renderQueue, const RenderableLighting& lighting, const bool debugLights);
        void ShadingPass(const RenderableLighting& lighting);
        void DebugPass(const DebugOptions::RenderingMode debugOptions);

        IMemoryAllocatorPtr mMemoryAllocator;
        Logger& mLogger;

        ShaderProgram mGeometryProgram;
        ShaderProgram mShadingProgram;
        GBuffer mGBuffer;
        GLuint mTextureSampler;
        float mCurrentAnisotropy;
        uint32_t mWindowWidth;
        uint32_t mWindowHeight;

        std::vector<OpenGLMeshPair> mMeshes;
        std::vector<OpenGLTexturePtr> mTextures;
        ShadingGeometry mShadingGeometry;
        UniformBuffer<OpenGLRenderer::UnifGeometry> mUniBufferGeometryPass;
        UniformBuffer<OpenGLRenderer::UnifShading> mUniBufferShadingPass;
    };
}