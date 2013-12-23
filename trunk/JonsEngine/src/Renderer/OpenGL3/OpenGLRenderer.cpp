#include "include/Renderer/OpenGL3/OpenGLRenderer.h"

#include "include/Renderer/OpenGL3/OpenGLMesh.h"
#include "include/Renderer/OpenGL3/OpenGLTexture.h"
#include "include/Renderer/OpenGL3/Shader.h"
#include "include/Renderer/OpenGL3/Shaders/GeometryVertexShader.h"
#include "include/Renderer/OpenGL3/Shaders/GeometryFragmentShader.h"
#include "include/Renderer/OpenGL3/Shaders/ShadingVertexShader.h"
#include "include/Renderer/OpenGL3/Shaders/ShadingFragmentShader.h"
#include "include/Renderer/OpenGL3/Shaders/StencilVertexShader.h"
#include "include/Renderer/OpenGL3/Shaders/StencilFragmentShader.h"
#include "include/Renderer/OpenGL3/OpenGLUtils.h"
#include "include/Renderer/Shapes.h"
#include "include/Core/EngineSettings.h"
#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/Utils/Math.h"

#include "GL/glew.h"
#include <exception>
#include <functional>
#include <utility>
#include <exception>

namespace JonsEngine
{
    const float Z_NEAR = 0.1f;
    const float Z_FAR  = 1000.0f;


    struct OpenGLRenderer::UnifGeometry
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
    
    struct OpenGLRenderer::UnifShading
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
        
        
        UnifShading(const Mat4& vwpMatrix, const Vec4& lightColor, const Vec4& lightPosOrDir, const Vec4& gamma, const Vec2& screenSize, const uint32_t lightType, const float falloffFactor, const float maxDistance)
            :
            mWVPMatrix(vwpMatrix), mLightColor(lightColor), mLightPosOrDir(lightPosOrDir), mGamma(gamma), mScreenSize(screenSize), mLightType(lightType), mFalloffFactor(falloffFactor), mMaxDistance(maxDistance)
        {
        }
    };
    
    struct OpenGLRenderer::UnifStencil
    {
        Mat4 mWVPMatrix;
        

        UnifStencil(const Mat4& wvpMatrix)
            : 
            mWVPMatrix(wvpMatrix)
        {
        }
    };


    void BindTexture2D(const OpenGLTexture::TextureUnit textureUnit, const TextureID targetTexture, const std::vector<OpenGLTexturePtr>& textures, Logger& logger)
    {
        GLCALL(glActiveTexture(GL_TEXTURE0 + textureUnit));

        // TODO: sort textures
        auto texture = std::find_if(textures.begin(), textures.end(), [&](const OpenGLTexturePtr ptr) { return ptr->mTextureID == targetTexture; });
        if (texture == textures.end())
        {
            JONS_LOG_ERROR(logger, "Renderable TextureID out of range");
            throw std::runtime_error("Renderable TextureID out of range");
        }

        GLCALL(glBindTexture(GL_TEXTURE_2D, texture->get()->mTexture));
    }

    void UnbindTexture2D(const OpenGLTexture::TextureUnit textureUnit, Logger& logger)
    {
        GLCALL(glActiveTexture(GL_TEXTURE0 + textureUnit));
        GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
    }


    OpenGLRenderer::OpenGLRenderer(const EngineSettings& engineSettings, IMemoryAllocatorPtr memoryAllocator) : OpenGLRenderer(engineSettings.mWindowWidth, engineSettings.mWindowHeight, engineSettings.mAnisotropicFiltering, memoryAllocator)
    {
    }

    OpenGLRenderer::OpenGLRenderer(OpenGLRenderer& renderer, IMemoryAllocatorPtr memoryAllocator) : OpenGLRenderer(renderer.mWindowWidth, renderer.mWindowHeight, renderer.mCurrentAnisotropy, memoryAllocator)
    {
        // move the meshes, recreate the VAO
        for (const OpenGLMeshPair mesh : renderer.mMeshes)
        {
            GLCALL(glDeleteVertexArrays(1, &mesh.second));

            mMeshes.emplace_back(mesh.first, SetupVAO(*mesh.first));
        }
        renderer.mMeshes.clear();

        for (const OpenGLTexturePtr texture : renderer.mTextures)
            mTextures.push_back(texture);
    }

    OpenGLRenderer::OpenGLRenderer(const uint32_t windowWidth, const uint32_t windowHeight, const float anisotropy, IMemoryAllocatorPtr memoryAllocator) :
        mMemoryAllocator(memoryAllocator), mLogger(Logger::GetRendererLogger()),
        mGeometryProgram("GeometryProgram", ShaderPtr(new Shader("GeometryVertexShader", gGeometryVertexShader, Shader::VERTEX_SHADER)),  ShaderPtr(new Shader("GeometryFragmentShader", gGeometryFragmentShader, Shader::FRAGMENT_SHADER)), mLogger),
        mShadingProgram("ShadingProgram", ShaderPtr(new Shader("ShadingVertexShader", gShadingVertexShader, Shader::VERTEX_SHADER)), ShaderPtr(new Shader("ShadingFragmentShader", gShadingFragmentShader, Shader::FRAGMENT_SHADER)), mLogger),
        mStencilProgram("StencilProgram", ShaderPtr(new Shader("StencilVertexShader", gStencilVertexShader, Shader::VERTEX_SHADER)), ShaderPtr(new Shader("StencilFragmentShader", gStencilFragmentShader, Shader::FRAGMENT_SHADER)), mLogger),
                                                            // VS2012 bug workaround. TODO: fixed in VS2013, when boost is rdy
        //mDefaultProgram("DefaultProgram", ShaderPtr(new Shader("DefaultVertexShader", gVertexShader, Shader::VERTEX_SHADER)/*mMemoryAllocator->AllocateObject<Shader>("DefaultVertexShader", gVertexShader, Shader::VERTEX_SHADER), [this](Shader* shader) { mMemoryAllocator->DeallocateObject(shader); }*/), 
        //                                 ShaderPtr(new Shader("DefaultFragmentShader", gFragmentShader, Shader::FRAGMENT_SHADER)/*mMemoryAllocator->AllocateObject<Shader>("DefaultFragmentShader", gFragmentShader, Shader::FRAGMENT_SHADER), [this](Shader* shader) { mMemoryAllocator->DeallocateObject(shader); })*/), mLogger),
        mGBuffer(mLogger, mShadingProgram.mProgramHandle, windowWidth, windowHeight), mTextureSampler(0), mCurrentAnisotropy(anisotropy), mWindowWidth(windowWidth), mWindowHeight(windowHeight),
        mShadingGeometry(mLogger), mUniBufferGeometryPass("UnifGeometry", mLogger, mGeometryProgram.mProgramHandle), mUniBufferShadingPass("UnifShading", mLogger, mShadingProgram.mProgramHandle),
        mUniBufferStencilPass("UnifStencil", mLogger, mStencilProgram.mProgramHandle)
    {
        GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));

        // face culling
	    GLCALL(glCullFace(GL_BACK));
	    GLCALL(glFrontFace(GL_CCW));

        // stencil testing
        GLCALL(glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP));
        GLCALL(glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP));

        // z depth testing
        GLCALL(glDepthFunc(GL_LEQUAL));
        GLCALL(glDepthRange(0.0f, 1.0f));
        GLCALL(glClearDepth(1.0f));

        // diffuse/normal texture sampler setup
        GLCALL(glUseProgram(mGeometryProgram.mProgramHandle));
        GLCALL(glGenSamplers(1, &mTextureSampler));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_T, GL_REPEAT));
        GLCALL(glBindSampler(OpenGLTexture::TEXTURE_UNIT_GEOMETRY_DIFFUSE, mTextureSampler));
        GLCALL(glBindSampler(OpenGLTexture::TEXTURE_UNIT_GEOMETRY_NORMAL, mTextureSampler));
        GLCALL(glUseProgram(0));

        SetAnisotropicFiltering(mCurrentAnisotropy);
    }

    OpenGLRenderer::~OpenGLRenderer()
    {
        for (const OpenGLMeshPair mesh : mMeshes)
            GLCALL(glDeleteVertexArrays(1, &mesh.second));

        GLCALL(glDeleteSamplers(1, &mTextureSampler));
    }


    MeshID OpenGLRenderer::CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData)
    {
        auto allocator = mMemoryAllocator;
        OpenGLMeshPtr meshPtr(allocator->AllocateObject<OpenGLMesh>(vertexData, normalData, texCoords, tangents, bitangents, indexData, mLogger), [=](OpenGLMesh* mesh) { allocator->DeallocateObject<OpenGLMesh>(mesh); });

        mMeshes.emplace_back(meshPtr, SetupVAO(*meshPtr));

        return mMeshes.back().first->mMeshID;
    }

    TextureID OpenGLRenderer::CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ColorFormat colorFormat)
    {
        auto allocator = mMemoryAllocator;
        mTextures.emplace_back(allocator->AllocateObject<OpenGLTexture>(textureData, textureWidth, textureHeight, colorFormat, textureType, mLogger), [=](OpenGLTexture* texture) { allocator->DeallocateObject<OpenGLTexture>(texture); });  // TODO: remove _VARIADIC_MAX 6 in MSVC12 with compatible boost
        
        return mTextures.back()->mTextureID;
    }


    void OpenGLRenderer::DrawRenderables(const RenderQueue& renderQueue, const RenderableLighting& lighting, const DebugOptions::RenderingMode debugMode, const DebugOptions::RenderingFlags debugExtra)
    {
        // clear default fbo color/buffer/stencil 
        GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

        GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBuffer.mFramebuffer));

        GeometryPass(renderQueue, lighting, debugExtra.test(DebugOptions::RENDER_FLAG_DRAW_LIGHTS));
        ShadingPass(lighting);

        GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
        GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBuffer.mFramebuffer));

        BlitToScreen(debugMode);

        GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
    }


    float OpenGLRenderer::GetMaxAnisotropicFiltering() const
    {
        float maxAniso = 1.0f;
        GLCALL(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso));

        return maxAniso;
    }

    float OpenGLRenderer::GetCurrentAnisotropicFiltering() const
    {
        return mCurrentAnisotropy;
    }
        
    void OpenGLRenderer::SetAnisotropicFiltering(const float newAnisoLevel)
    {
        const float maxAnisoFiltering = GetMaxAnisotropicFiltering();

        if (newAnisoLevel > maxAnisoFiltering)
            mCurrentAnisotropy = maxAnisoFiltering;
        else if (newAnisoLevel < 1.0f)
            mCurrentAnisotropy = 1.0f;
        else
            mCurrentAnisotropy = newAnisoLevel;

        GLCALL(glSamplerParameterf(mTextureSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, mCurrentAnisotropy));
    }


    float OpenGLRenderer::GetZNear() const
    { 
        return Z_NEAR; 
    }
    
    float OpenGLRenderer::GetZFar() const
    { 
        return Z_FAR;
    }


    void OpenGLRenderer::GeometryPass(const RenderQueue& renderQueue, const RenderableLighting& lighting, const bool debugLights)
    {
        GLCALL(glUseProgram(mGeometryProgram.mProgramHandle));
        GLCALL(glEnable(GL_CULL_FACE));
        GLCALL(glDepthMask(GL_TRUE));
        GLCALL(glEnable(GL_DEPTH_TEST));

        // set geometry pass output buffers
        GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        GLCALL(glDrawBuffers(GBuffer::GBUFFER_NUM_TEXTURES, drawBuffers));

        // clear GBuffer position/normal/diffuse textures and depth buffer
        GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        // both containers are assumed to be sorted by MeshID ascending
        auto meshIterator = mMeshes.begin();
        for (const Renderable& renderable : renderQueue)
        {
            if (renderable.mMesh == INVALID_MESH_ID)
            {
                JONS_LOG_ERROR(mLogger, "Renderable MeshID is invalid");
                throw std::runtime_error("Renderable MeshID is invalid");
            }

            if (renderable.mMesh < meshIterator->first->mMeshID)
                continue;

            while (renderable.mMesh > meshIterator->first->mMeshID)
            {
                meshIterator++;
                if (meshIterator == mMeshes.end())
                {
                    JONS_LOG_ERROR(mLogger, "Renderable MeshID out of range");
                    throw std::runtime_error("Renderable MeshID out of range");
                }
            }

            bool hasDiffuseTexture = renderable.mDiffuseTexture != INVALID_TEXTURE_ID;
            bool hasNormalTexture = renderable.mNormalTexture != INVALID_TEXTURE_ID;

            mUniBufferGeometryPass.SetData(UnifGeometry(renderable.mWVPMatrix, renderable.mWorldMatrix, hasDiffuseTexture, hasNormalTexture, renderable.mTextureTilingFactor));

            if (hasDiffuseTexture)
                BindTexture2D(OpenGLTexture::TEXTURE_UNIT_GEOMETRY_DIFFUSE, renderable.mDiffuseTexture, mTextures, mLogger);

            if (hasNormalTexture)
                BindTexture2D(OpenGLTexture::TEXTURE_UNIT_GEOMETRY_NORMAL, renderable.mNormalTexture, mTextures, mLogger);

            GLCALL(glBindVertexArray(meshIterator->second));
            GLCALL(glDrawElements(GL_TRIANGLES, meshIterator->first->mIndices, GL_UNSIGNED_INT, 0));
            GLCALL(glBindVertexArray(0));

            if (hasDiffuseTexture)
                UnbindTexture2D(OpenGLTexture::TEXTURE_UNIT_GEOMETRY_DIFFUSE, mLogger);

            if (hasNormalTexture)
                UnbindTexture2D(OpenGLTexture::TEXTURE_UNIT_GEOMETRY_NORMAL, mLogger);
        }

        // debug: draw all lights
        if (debugLights)
        {
            for (const RenderableLighting::PointLight& pointLight : lighting.mPointLights)
            {
                mUniBufferGeometryPass.SetData(UnifGeometry(pointLight.mWVPMatrix, pointLight.mWorldMatrix, false, false, 1.0f));
                mShadingGeometry.DrawSphere();
            }
        }

        GLCALL(glDrawBuffer(GL_NONE));
        GLCALL(glDisable(GL_DEPTH_TEST));
        GLCALL(glDepthMask(GL_FALSE));
        GLCALL(glDisable(GL_CULL_FACE));
        GLCALL(glUseProgram(0));
    }
        
    void OpenGLRenderer::ShadingPass(const RenderableLighting& lighting)
    {
        GLCALL(glUseProgram(mShadingProgram.mProgramHandle));
        GLCALL(glDrawBuffer(GL_COLOR_ATTACHMENT0 + GBuffer::GBUFFER_NUM_TEXTURES));

        mGBuffer.BindGBufferTextures();

        // clear final texture buffer
        GLCALL(glClear(GL_COLOR_BUFFER_BIT));

        // draw fullscreen rect for ambient light
        mUniBufferShadingPass.SetData(UnifShading(Mat4(1.0f), lighting.mAmbientLight, Vec4(0.0f), lighting.mGamma, lighting.mScreenSize, UnifShading::LIGHT_TYPE_AMBIENT, 0.0f, 0.0f));
        mShadingGeometry.DrawRectangle();

        // for directional and point lights
        GLCALL(glEnable(GL_BLEND));
        GLCALL(glBlendEquation(GL_FUNC_ADD));
        GLCALL(glBlendFunc(GL_ONE, GL_ONE));

        // do all directional lights
        for (const RenderableLighting::DirectionalLight& directionalLight : lighting.mDirectionalLights)
        {
            mUniBufferShadingPass.SetData(UnifShading(Mat4(1.0f), directionalLight.mLightColor, directionalLight.mLightDirection, lighting.mGamma, lighting.mScreenSize, UnifShading::LIGHT_TYPE_DIRECTIONAL, 0.0f, 0.0f));
            mShadingGeometry.DrawRectangle();
        }

        // do all point lights
        GLCALL(glEnable(GL_STENCIL_TEST));
        for (const RenderableLighting::PointLight& pointLight : lighting.mPointLights)
        {
            GLCALL(glClear(GL_STENCIL_BUFFER_BIT));
        
            GLCALL(glDrawBuffer(GL_NONE));
            PointLightStencilPass(pointLight);
        
            GLCALL(glDrawBuffer(GL_COLOR_ATTACHMENT0 + GBuffer::GBUFFER_NUM_TEXTURES));
            PointLightLightingPass(pointLight, lighting.mGamma, lighting.mScreenSize);
        }
        GLCALL(glDisable(GL_STENCIL_TEST));

        // reset state
        GLCALL(glDrawBuffer(GL_NONE));
        GLCALL(glDisable(GL_BLEND));
        GLCALL(glUseProgram(0));
    }

    void OpenGLRenderer::PointLightStencilPass(const RenderableLighting::PointLight& pointLight)
    {
        GLCALL(glUseProgram(mStencilProgram.mProgramHandle));
        GLCALL(glEnable(GL_DEPTH_TEST));
        GLCALL(glStencilFunc(GL_ALWAYS, 0, 0));
        
        mUniBufferStencilPass.SetData(UnifStencil(pointLight.mWVPMatrix));
        mShadingGeometry.DrawSphere();
        
        GLCALL(glDisable(GL_DEPTH_TEST));
    }

    void OpenGLRenderer::PointLightLightingPass(const RenderableLighting::PointLight& pointLight, const Vec4& gamma, const Vec2& screenSize)
    {
        GLCALL(glUseProgram(mShadingProgram.mProgramHandle));
        GLCALL(glEnable(GL_CULL_FACE));
        GLCALL(glCullFace(GL_FRONT));
        GLCALL(glStencilFunc(GL_NOTEQUAL, 0, 0xFF));

        mUniBufferShadingPass.SetData(UnifShading(pointLight.mWVPMatrix, pointLight.mLightColor, pointLight.mLightPosition, gamma, screenSize, UnifShading::LIGHT_TYPE_POINT, pointLight.mFalloffFactor, pointLight.mMaxDistance));
        mShadingGeometry.DrawSphere();

        GLCALL(glCullFace(GL_BACK));
        GLCALL(glDisable(GL_CULL_FACE));
    }

    void OpenGLRenderer::BlitToScreen(const DebugOptions::RenderingMode debugOptions)
    {
        switch (debugOptions)
        {
            case DebugOptions::RENDER_MODE_FULL:
            {
                GLCALL(glReadBuffer(GL_COLOR_ATTACHMENT0 + GBuffer::GBUFFER_NUM_TEXTURES));
                GLCALL(glBlitFramebuffer(0, 0, mWindowWidth, mWindowHeight, 0, 0, mWindowWidth, mWindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR));
                break;
            }
            case DebugOptions::RENDER_MODE_POSITIONS:
            {
                GLCALL(glReadBuffer(GL_COLOR_ATTACHMENT0 + GBuffer::GBUFFER_TEXTURE_POSITION));
                GLCALL(glBlitFramebuffer(0, 0, mWindowWidth, mWindowHeight, 0, 0, mWindowWidth, mWindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR));
                break;
            }
            case DebugOptions::RENDER_MODE_NORMALS:
            {
                GLCALL(glReadBuffer(GL_COLOR_ATTACHMENT0 + GBuffer::GBUFFER_TEXTURE_NORMAL));
                GLCALL(glBlitFramebuffer(0, 0, mWindowWidth, mWindowHeight, 0, 0, mWindowWidth, mWindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR));
                break;
            }
            case DebugOptions::RENDER_MODE_DIFFUSE:
            {
                GLCALL(glReadBuffer(GL_COLOR_ATTACHMENT0 + GBuffer::GBUFFER_TEXTURE_DIFFUSE));
                GLCALL(glBlitFramebuffer(0, 0, mWindowWidth, mWindowHeight, 0, 0, mWindowWidth, mWindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR));
                break;
            }
            default:
                break;
        }
    }
}