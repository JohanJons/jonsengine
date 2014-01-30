#include "include/Renderer/OpenGL3/OpenGLRenderer.h"

#include "include/Renderer/OpenGL3/OpenGLMesh.h"
#include "include/Renderer/OpenGL3/OpenGLTexture.h"
#include "include/Renderer/OpenGL3/Shader.h"
#include "include/Renderer/OpenGL3/Shaders/GeometryVertexShader.h"
#include "include/Renderer/OpenGL3/Shaders/GeometryFragmentShader.h"
#include "include/Renderer/OpenGL3/Shaders/NullVertexShader.h"
#include "include/Renderer/OpenGL3/Shaders/NullFragmentShader.h"
#include "include/Renderer/OpenGL3/Shaders/AmbientLightVertexShader.h"
#include "include/Renderer/OpenGL3/Shaders/AmbientLightFragmentShader.h"
#include "include/Renderer/OpenGL3/Shaders/PointLightVertexShader.h"
#include "include/Renderer/OpenGL3/Shaders/PointLightFragmentShader.h"
#include "include/Renderer/OpenGL3/Shaders/DirLightVertexShader.h"
#include "include/Renderer/OpenGL3/Shaders/DirLightFragmentShader.h"
#include "include/Renderer/OpenGL3/Shaders/DepthVertexShader.h"
#include "include/Renderer/OpenGL3/Shaders/DepthFragmentShader.h"
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
    const float Z_FAR  = 100.0f;

    const Mat4 gNDCBiasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
                              0.0f, 0.5f, 0.0f, 0.0f,
                              0.0f, 0.0f, 0.5f, 0.0f,
                              0.5f, 0.5f, 0.5f, 1.0f);


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

    OpenGLRenderer::OpenGLRenderer(const std::vector<OpenGLMeshPtr>& meshes, const std::vector<OpenGLTexturePtr>& textures, const uint32_t windowWidth, const uint32_t windowHeight, const float anisotropy, IMemoryAllocatorPtr memoryAllocator)
        : 
        OpenGLRenderer(windowWidth, windowHeight, anisotropy, memoryAllocator)
    {
        // recreate the VAOs
        for (const OpenGLMeshPtr mesh : meshes)
            mMeshes.emplace_back(mesh, SetupVAO(*mesh));

        mTextures = textures;
    }

    OpenGLRenderer::OpenGLRenderer(const uint32_t windowWidth, const uint32_t windowHeight, const float anisotropy, IMemoryAllocatorPtr memoryAllocator) :
        mMemoryAllocator(memoryAllocator), mLogger(Logger::GetRendererLogger()),
        mGeometryProgram("GeometryProgram", ShaderPtr(new Shader("GeometryVertexShader", gGeometryVertexShader, Shader::VERTEX_SHADER)), ShaderPtr(new Shader("GeometryFragmentShader", gGeometryFragmentShader, Shader::FRAGMENT_SHADER)), mLogger, "UnifGeometry"),
        mPointLightProgram("PointLightProgram", ShaderPtr(new Shader("PointLightVertexShader", gShadingVertexShader, Shader::VERTEX_SHADER)), ShaderPtr(new Shader("PointLightFragmentShader", gShadingFragmentShader, Shader::FRAGMENT_SHADER)), mLogger, "UnifPointLight"),
        mNullProgram("NullProgram", ShaderPtr(new Shader("NullVertexShader", gNullVertexShader, Shader::VERTEX_SHADER)), ShaderPtr(new Shader("NullFragmentShader", gNullFragmentShader, Shader::FRAGMENT_SHADER)), mLogger, "UnifNull"),
        mDirLightProgram("DirLightProgram", ShaderPtr(new Shader("DirLightVertexShader", gDirLightVertexShader, Shader::VERTEX_SHADER)), ShaderPtr(new Shader("DirLightFragmentShader", gDirLightFragmentShader, Shader::FRAGMENT_SHADER)), mLogger, "UnifDirLight"),
        mAmbientProgram("AmbientLightProgram", ShaderPtr(new Shader("AmbientLightVertexShader", gAmbientVertexShader, Shader::VERTEX_SHADER)), ShaderPtr(new Shader("AmbientLightFragmentShader", gAmbientFragmentShader, Shader::FRAGMENT_SHADER)), mLogger, "UnifAmbient"),
        mDepthProgram("DepthProgram", ShaderPtr(new Shader("DepthVertexShader", gDepthVertexShader, Shader::VERTEX_SHADER)), ShaderPtr(new Shader("DepthFragmentShader", gDepthFragmentShader, Shader::FRAGMENT_SHADER)), mLogger, "UnifDepth"),
                                                            // VS2012 bug workaround. TODO: fixed in VS2013, when boost is rdy
        //mDefaultProgram("DefaultProgram", ShaderPtr(new Shader("DefaultVertexShader", gVertexShader, Shader::VERTEX_SHADER)/*mMemoryAllocator->AllocateObject<Shader>("DefaultVertexShader", gVertexShader, Shader::VERTEX_SHADER), [this](Shader* shader) { mMemoryAllocator->DeallocateObject(shader); }*/), 
        //                                 ShaderPtr(new Shader("DefaultFragmentShader", gFragmentShader, Shader::FRAGMENT_SHADER)/*mMemoryAllocator->AllocateObject<Shader>("DefaultFragmentShader", gFragmentShader, Shader::FRAGMENT_SHADER), [this](Shader* shader) { mMemoryAllocator->DeallocateObject(shader); })*/), mLogger),
        mGBuffer(mLogger, windowWidth, windowHeight), mTextureSampler(0), mCurrentAnisotropy(anisotropy), mWindowWidth(windowWidth), mWindowHeight(windowHeight),
        mShadingGeometry(mLogger), mDirectionalShadowMap(mLogger, windowWidth, windowHeight)
    {
        GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));

        // face culling
	    GLCALL(glCullFace(GL_BACK));
	    GLCALL(glFrontFace(GL_CCW));

        // stencil testing
        GLCALL(glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP));
        GLCALL(glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP));

        // blend
        GLCALL(glBlendEquation(GL_FUNC_ADD));
        GLCALL(glBlendFunc(GL_ONE, GL_ONE));

        // z depth testing
        GLCALL(glDepthFunc(GL_LEQUAL));
        GLCALL(glDepthRange(0.0f, 1.0f));
        GLCALL(glClearDepth(1.0f));

        // diffuse/normal texture sampler setup
        GLCALL(glGenSamplers(1, &mTextureSampler));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_T, GL_REPEAT));
        GLCALL(glBindSampler(OpenGLTexture::TEXTURE_UNIT_GEOMETRY_DIFFUSE, mTextureSampler));
        GLCALL(glBindSampler(OpenGLTexture::TEXTURE_UNIT_GEOMETRY_NORMAL, mTextureSampler));
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
        ShadingPass(renderQueue, lighting);

        GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
        GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBuffer.mFramebuffer));

        RenderToScreen(debugMode, lighting.mScreenSize);

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


    std::vector<OpenGLMeshPtr> OpenGLRenderer::GetMeshes() const
    {
        std::vector<OpenGLMeshPtr> ret;
        ret.reserve(mMeshes.size());

        for (const OpenGLMeshPair meshPair : mMeshes)
            ret.push_back(meshPair.first);

        return ret;
    }

    std::vector<OpenGLTexturePtr> OpenGLRenderer::GetTextures() const
    {
        return std::vector<OpenGLTexturePtr>(mTextures);
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
        mGeometryProgram.UseProgram();
        GLCALL(glEnable(GL_CULL_FACE));
        GLCALL(glEnable(GL_DEPTH_TEST));
        GLCALL(glDepthMask(GL_TRUE));

        // set geometry pass output buffers
        GLenum drawBuffers[] = { GBuffer::GBUFFER_COLOR_ATTACHMENT_POSITION, GBuffer::GBUFFER_COLOR_ATTACHMENT_NORMAL, GBuffer::GBUFFER_COLOR_ATTACHMENT_DIFFUSE };
        GLCALL(glDrawBuffers(GBuffer::GBUFFER_NUM_GEOMETRY_ATTACHMENTS, drawBuffers));

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

            mGeometryProgram.SetUniformData(UnifGeometry(renderable.mWVPMatrix, renderable.mWorldMatrix, hasDiffuseTexture, hasNormalTexture, renderable.mTextureTilingFactor));

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
                mGeometryProgram.SetUniformData(UnifGeometry(pointLight.mWVPMatrix, pointLight.mWorldMatrix, false, false, 1.0f));
                mShadingGeometry.DrawSphere();
            }
        }

        GLCALL(glDrawBuffer(GL_NONE));
        GLCALL(glDisable(GL_DEPTH_TEST));
        GLCALL(glDisable(GL_CULL_FACE));
        GLCALL(glDepthMask(GL_FALSE));
        GLCALL(glUseProgram(0));
    }
        
    void OpenGLRenderer::ShadingPass(const RenderQueue& renderQueue, const RenderableLighting& lighting)
    {
        GLCALL(glDrawBuffer(GBuffer::GBUFFER_COLOR_ATTACHMENT_FINAL));
        mGBuffer.BindGeometryTextures();

        // clear final texture buffer
        GLCALL(glClear(GL_COLOR_BUFFER_BIT));

        // draw fullscreen rect for ambient light
        AmbientLightPass(lighting.mAmbientLight, lighting.mGamma, lighting.mScreenSize);

        // for directional and point lights
        GLCALL(glEnable(GL_BLEND));

        // do all directional lights
        for (const RenderableLighting::DirectionalLight& directionalLight : lighting.mDirectionalLights)
        {
            Mat4 viewMatrix = glm::lookAt(lighting.mCameraPosition, lighting.mCameraPosition + glm::normalize(directionalLight.mLightDirection), Vec3(0.0f, 1.0f, 0.0f));
            Mat4 lightVP = glm::ortho(lighting.mCameraPosition.x - 25.0f, lighting.mCameraPosition.x + 25.0f, lighting.mCameraPosition.y - 25.0f, lighting.mCameraPosition.y + 25.0f, lighting.mCameraPosition.z + 25.0f, lighting.mCameraPosition.z - 25.0f) * viewMatrix;

            GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mDirectionalShadowMap.mFramebuffer));
            GLCALL(glDrawBuffer(GL_NONE));
            DirLightShadowPass(directionalLight, lightVP, renderQueue);

            GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBuffer.mFramebuffer));
            GLCALL(glDrawBuffer(GBuffer::GBUFFER_COLOR_ATTACHMENT_FINAL));
            DirLightLightingPass(directionalLight, gNDCBiasMatrix * lightVP, lighting.mGamma, lighting.mScreenSize);
        }

        // do all point lights
        GLCALL(glEnable(GL_STENCIL_TEST));
        for (const RenderableLighting::PointLight& pointLight : lighting.mPointLights)
        {
            // stencil pass first to elimiate fragments that dosnt need to be lit
            GLCALL(glDrawBuffer(GL_NONE));
            PointLightStencilPass(pointLight);
        
            GLCALL(glDrawBuffer(GBuffer::GBUFFER_COLOR_ATTACHMENT_FINAL));
            PointLightLightingPass(pointLight, lighting.mGamma, lighting.mScreenSize);
        }
        GLCALL(glDisable(GL_STENCIL_TEST));

        // reset state
        GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
        GLCALL(glDisable(GL_BLEND));
        GLCALL(glDrawBuffer(GL_NONE));
        GLCALL(glUseProgram(0));
    }

    void OpenGLRenderer::AmbientLightPass(const Vec4& ambientLight, const Vec4& gamma, const Vec2& screenSize)
    {
        mAmbientProgram.UseProgram();

        mAmbientProgram.SetUniformData(UnifAmbientLight(ambientLight, gamma, screenSize));
        mShadingGeometry.DrawRectangle();

        GLCALL(glUseProgram(0));
    }

    void OpenGLRenderer::PointLightStencilPass(const RenderableLighting::PointLight& pointLight)
    {
        mNullProgram.UseProgram();
        GLCALL(glEnable(GL_DEPTH_TEST));
        GLCALL(glStencilFunc(GL_ALWAYS, 0, 0));

        GLCALL(glClear(GL_STENCIL_BUFFER_BIT));
        
        mNullProgram.SetUniformData(UnifNull(pointLight.mWVPMatrix));
        mShadingGeometry.DrawSphere();
        
        GLCALL(glDisable(GL_DEPTH_TEST));
        GLCALL(glUseProgram(0));
    }

    void OpenGLRenderer::PointLightLightingPass(const RenderableLighting::PointLight& pointLight, const Vec4& gamma, const Vec2& screenSize)
    {
        mPointLightProgram.UseProgram();
        GLCALL(glEnable(GL_CULL_FACE));
        GLCALL(glCullFace(GL_FRONT));   // cull FRONT face so the light dosnt get culled when inside the radius
        GLCALL(glStencilFunc(GL_NOTEQUAL, 0, 0xFF));

        mPointLightProgram.SetUniformData(UnifPointLight(pointLight.mWVPMatrix, pointLight.mLightColor, Vec4(pointLight.mLightPosition, 1.0f), gamma, screenSize, pointLight.mLightIntensity, pointLight.mMaxDistance));
        mShadingGeometry.DrawSphere();

        GLCALL(glCullFace(GL_BACK));
        GLCALL(glDisable(GL_CULL_FACE));
        GLCALL(glUseProgram(0));
    }

    void OpenGLRenderer::DirLightShadowPass(const RenderableLighting::DirectionalLight& dirLight, const Mat4& lightVP, const RenderQueue& renderQueue)
    {
        mNullProgram.UseProgram();
        GLCALL(glEnable(GL_CULL_FACE));
        GLCALL(glEnable(GL_DEPTH_TEST));
        GLCALL(glDepthMask(GL_TRUE));

        GLCALL(glClear(GL_DEPTH_BUFFER_BIT));

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

            Mat4 wvp = lightVP * renderable.mWorldMatrix;
            mNullProgram.SetUniformData(UnifNull(wvp));

            GLCALL(glBindVertexArray(meshIterator->second));
            GLCALL(glDrawElements(GL_TRIANGLES, meshIterator->first->mIndices, GL_UNSIGNED_INT, 0));
            GLCALL(glBindVertexArray(0));
        }

        GLCALL(glDisable(GL_DEPTH_TEST));
        GLCALL(glDisable(GL_CULL_FACE));
        GLCALL(glDepthMask(GL_FALSE));
        GLCALL(glUseProgram(0));
    }

    void OpenGLRenderer::DirLightLightingPass(const RenderableLighting::DirectionalLight& dirLight, const Mat4& lightVP, const Vec4& gamma, const Vec2& screenSize)
    {
        mDirLightProgram.UseProgram();

        mDirectionalShadowMap.BindShadowMap();

        mDirLightProgram.SetUniformData(UnifDirLight(lightVP, dirLight.mLightColor, Vec4(dirLight.mLightDirection, 0.0f), gamma, screenSize));
        mShadingGeometry.DrawRectangle();

        GLCALL(glUseProgram(0));
    }

    void OpenGLRenderer::RenderToScreen(const DebugOptions::RenderingMode debugOptions, const Vec2& screenSize)
    {
        switch (debugOptions)
        {
            case DebugOptions::RENDER_MODE_FULL:
            {
                GLCALL(glReadBuffer(GBuffer::GBUFFER_COLOR_ATTACHMENT_FINAL));
                GLCALL(glBlitFramebuffer(0, 0, mWindowWidth, mWindowHeight, 0, 0, mWindowWidth, mWindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR));
                break;
            }
            case DebugOptions::RENDER_MODE_POSITIONS:
            {
                GLCALL(glReadBuffer(GBuffer::GBUFFER_COLOR_ATTACHMENT_POSITION));
                GLCALL(glBlitFramebuffer(0, 0, mWindowWidth, mWindowHeight, 0, 0, mWindowWidth, mWindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR));
                break;
            }
            case DebugOptions::RENDER_MODE_NORMALS:
            {
                GLCALL(glReadBuffer(GBuffer::GBUFFER_COLOR_ATTACHMENT_NORMAL));
                GLCALL(glBlitFramebuffer(0, 0, mWindowWidth, mWindowHeight, 0, 0, mWindowWidth, mWindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR));
                break;
            }
            case DebugOptions::RENDER_MODE_DIFFUSE:
            {
                GLCALL(glReadBuffer(GBuffer::GBUFFER_COLOR_ATTACHMENT_DIFFUSE));
                GLCALL(glBlitFramebuffer(0, 0, mWindowWidth, mWindowHeight, 0, 0, mWindowWidth, mWindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR));
                break;
            }
            case DebugOptions::RENDER_MODE_DEPTH:
            {
                mDepthProgram.UseProgram();
                mDepthProgram.SetUniformData(UnifDepth(screenSize, Z_NEAR, Z_FAR));
                mGBuffer.BindDepthTexture();
                mShadingGeometry.DrawRectangle();
                break;
            }
            default:
                break;
        }
    }
}