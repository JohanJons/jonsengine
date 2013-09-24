#pragma once

#include "include/Renderer/IRenderer.h"
#include "include/Renderer/Renderable.h"
#include "include/Renderer/RenderableLighting.h"
#include "include/Renderer/OpenGL3/ShaderProgram.h"
#include "include/Renderer/OpenGL3/UniformBuffer.hpp"

#include <string>
#include <vector>
#include <memory>

namespace JonsEngine
{
    class Logger;
    class OpenGLRenderer;
    struct EngineSettings;

    /* OpenGLRendererPtr definition */
    typedef std::unique_ptr<OpenGLRenderer, void(*)(OpenGLRenderer* backend)> ManagedOpenGLRenderer;
    typedef ManagedOpenGLRenderer& OpenGLRendererPtr;

    /* OpenGLRenderer definition */
    class OpenGLRenderer : public IRenderer
    {
    public:
        OpenGLRenderer(const EngineSettings& engineSettings);
        OpenGLRenderer(const float anisotropicLevel);
        ~OpenGLRenderer();

        MeshPtr CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<uint32_t>& indexData);
        TexturePtr CreateTexture(ITexture::TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ITexture::TextureFormat textureFormat);

        void DrawRenderables(const RenderQueue& renderQueue, const RenderableLighting& lighting);
        RenderBackendType GetRenderBackendType() const;

        float GetMaxAnisotropicFiltering() const;
        float GetCurrentAnisotropicFiltering() const;
        bool SetAnisotropicFiltering(const float newAnisoLevel);

        float GetZNear() const;
        float GetZFar() const;


    private:
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
            int32_t mHasDiffuseTexture;
            int32_t mLightingEnabled;
            float mSpecularFactor;

            Material(const Vec4& diffuseColor, const Vec4& ambientColor, const Vec4& specularColor, const Vec4& emissiveColor, const int32_t hasDiffuseTexture, const int32_t lightingEnabled, const float specularFactor) 
                : 
                mDiffuseColor(diffuseColor), mAmbientColor(ambientColor), mSpecularColor(specularColor),  mEmissiveColor(emissiveColor), mHasDiffuseTexture(hasDiffuseTexture), mLightingEnabled(lightingEnabled), mSpecularFactor(specularFactor)
            {
            }
        };


        ShaderProgram mDefaultProgram;
        UniformBuffer<OpenGLRenderer::Transform> mUniBufferTransform;
        UniformBuffer<OpenGLRenderer::Material> mUniBufferMaterial;
        UniformBuffer<RenderableLighting> mUniBufferLightingInfo;
        GLuint mTextureSampler;
        float mCurrentAnisotropy;
        bool mFirstPass;
        Logger& mLogger;
    };


    /* OpenGLRenderBackend inlines */
    inline IRenderer::RenderBackendType OpenGLRenderer::GetRenderBackendType() const       { return OPENGL; }
    inline float OpenGLRenderer::GetZNear() const                                          { return 0.5f; }
    inline float OpenGLRenderer::GetZFar() const                                           { return 1000.0f; }
}