#pragma once

#include "include/Renderer/IRenderer.h"
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
        TexturePtr CreateTexture(ITexture::TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ITexture::TextureFormat textureFormat);

        void DrawRenderables(const RenderQueue& renderQueue, const RenderableLighting& lighting);
        RenderBackendType GetRenderBackendType() const;


    private:
        struct Transform
        {
            Mat4 mWVPMatrix;
            Mat4 mWorldMatrix;

            Transform(const Mat4& WVPMatrix, const Mat4& WorldMatrix) : mWVPMatrix(WVPMatrix), mWorldMatrix(WorldMatrix)
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
            float mSpecularFactor;

            Material(const Vec4& diffuseColor, const Vec4& ambientColor, const Vec4& specularColor, const Vec4& emissiveColor, const int32_t hasDiffuseTexture, const float specularFactor) 
                : 
                mDiffuseColor(diffuseColor), mAmbientColor(ambientColor), mSpecularColor(specularColor),  mEmissiveColor(emissiveColor), mHasDiffuseTexture(hasDiffuseTexture), mSpecularFactor(specularFactor)
            {
            }
        };

        ShaderProgram SetupShaderProgram(const std::string& programName);

        ShaderProgram mDefaultProgram;
        UniformBuffer<OpenGLRenderer::Transform> mUniBufferTransform;
        UniformBuffer<OpenGLRenderer::Material> mUniBufferMaterial;
        UniformBuffer<RenderableLighting> mUniBufferLightingInfo;
        GLuint mTextureSampler;

        Logger& mLogger;
    };


    /* OpenGLRenderBackend inlines */
    inline IRenderer::RenderBackendType OpenGLRenderer::GetRenderBackendType() const       { return OPENGL; }
}