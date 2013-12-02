#include "include/Renderer/OpenGL3/OpenGLRenderer.h"

#include "include/Renderer/OpenGL3/OpenGLMesh.h"
#include "include/Renderer/OpenGL3/OpenGLTexture.h"
#include "include/Renderer/OpenGL3/Shader.h"
#include "include/Renderer/OpenGL3/Shaders/GeometryVertexShader.h"
#include "include/Renderer/OpenGL3/Shaders/GeometryFragmentShader.h"
#include "include/Renderer/OpenGL3/OpenGLUtils.hpp"
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

    GLuint SetupVAO(OpenGLMeshPtr mesh)
    {
        GLuint vao;

        GLCALL(glGenVertexArrays(1, &vao));

        GLCALL(glBindVertexArray(vao));
        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, mesh->mVBO));
        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->mIndexBuffer));

        GLCALL(glEnableVertexAttribArray(0));
        GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0));
        GLCALL(glEnableVertexAttribArray(1));
        GLCALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(mesh->mVertexDataSize)));
        GLCALL(glEnableVertexAttribArray(2));
        GLCALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(mesh->mVertexDataSize + mesh->mNormalDataSize)));
        GLCALL(glEnableVertexAttribArray(3));
        GLCALL(glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(mesh->mVertexDataSize + mesh->mNormalDataSize + mesh->mTexCoordsSize)));
        GLCALL(glEnableVertexAttribArray(4));
        GLCALL(glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(mesh->mVertexDataSize + mesh->mNormalDataSize + mesh->mTexCoordsSize + mesh->mTangentsSize)));

        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCALL(glBindVertexArray(0));
        
        return vao;
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

            mMeshes.emplace_back(mesh.first, SetupVAO(mesh.first));
        }
        renderer.mMeshes.clear();

        for (const OpenGLTexturePtr texture : renderer.mTextures)
            mTextures.push_back(texture);
    }

    OpenGLRenderer::OpenGLRenderer(const uint32_t windowWidth, const uint32_t windowHeight, const float anisotropy, IMemoryAllocatorPtr memoryAllocator) :
        mMemoryAllocator(memoryAllocator), mLogger(Logger::GetRendererLogger()),
        mGeometryProgram("GeometryProgram", ShaderPtr(new Shader("GeometryVertexShader", gGeometryVertexShader, Shader::VERTEX_SHADER)),  ShaderPtr(new Shader("GeometryFragmentShader", gGeometryFragmentShader, Shader::FRAGMENT_SHADER)), mLogger),
        mShadingProgram("ShadingProgram", ShaderPtr(new Shader("ShadingVertexShader", gGeometryVertexShader, Shader::VERTEX_SHADER)), ShaderPtr(new Shader("ShadingFragmentShader", gGeometryFragmentShader, Shader::FRAGMENT_SHADER)), mLogger),
        mDebugProgram("DebugProgram", ShaderPtr(new Shader("DebugVertexShader", gGeometryVertexShader, Shader::VERTEX_SHADER)), ShaderPtr(new Shader("DebugFragmentShader", gGeometryFragmentShader, Shader::FRAGMENT_SHADER)), mLogger),
                                                            // VS2012 bug workaround. TODO: fixed in VS2013, when boost is rdy
        //mDefaultProgram("DefaultProgram", ShaderPtr(new Shader("DefaultVertexShader", gVertexShader, Shader::VERTEX_SHADER)/*mMemoryAllocator->AllocateObject<Shader>("DefaultVertexShader", gVertexShader, Shader::VERTEX_SHADER), [this](Shader* shader) { mMemoryAllocator->DeallocateObject(shader); }*/), 
        //                                 ShaderPtr(new Shader("DefaultFragmentShader", gFragmentShader, Shader::FRAGMENT_SHADER)/*mMemoryAllocator->AllocateObject<Shader>("DefaultFragmentShader", gFragmentShader, Shader::FRAGMENT_SHADER), [this](Shader* shader) { mMemoryAllocator->DeallocateObject(shader); })*/), mLogger),
        mGBuffer(mLogger, mGeometryProgram.mProgramHandle, windowWidth, windowHeight), mTextureSampler(0), mCurrentAnisotropy(anisotropy), mWindowWidth(windowWidth), mWindowHeight(windowHeight),
        mUniBufferGeometryPass("UnifGeometry", mLogger, mGeometryProgram.mProgramHandle)
    {
        GLCALL(glClearDepth(1.0f));
        GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

        // face culling
        GLCALL(glEnable(GL_CULL_FACE));
	    GLCALL(glCullFace(GL_BACK));
	    GLCALL(glFrontFace(GL_CCW));

        // z depth testing
        GLCALL(glEnable(GL_DEPTH_TEST));
        GLCALL(glDepthMask(GL_TRUE));
        GLCALL(glDepthFunc(GL_LEQUAL));
        GLCALL(glDepthRange(0.0f, 1.0f));

        // diffuse/normal texture sampler setup
        GLCALL(glUseProgram(mGeometryProgram.mProgramHandle));
        GLCALL(glGenSamplers(1, &mTextureSampler));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_T, GL_REPEAT));
        GLCALL(glUniform1i(glGetUniformLocation(mGeometryProgram.mProgramHandle, "unifDiffuseTexture"), OpenGLTexture::TEXTURE_UNIT_DIFFUSE));
        GLCALL(glUniform1i(glGetUniformLocation(mGeometryProgram.mProgramHandle, "unifNormalTexture"), OpenGLTexture::TEXTURE_UNIT_NORMAL));
        GLCALL(glBindSampler(OpenGLTexture::TEXTURE_UNIT_DIFFUSE, mTextureSampler));
        GLCALL(glBindSampler(OpenGLTexture::TEXTURE_UNIT_NORMAL, mTextureSampler));
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

        mMeshes.emplace_back(meshPtr, SetupVAO(meshPtr));

        return mMeshes.back().first->mMeshID;
    }

    TextureID OpenGLRenderer::CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ColorFormat colorFormat)
    {
        auto allocator = mMemoryAllocator;
        mTextures.emplace_back(allocator->AllocateObject<OpenGLTexture>(textureData, textureWidth, textureHeight, colorFormat, textureType, mLogger), [=](OpenGLTexture* texture) { allocator->DeallocateObject<OpenGLTexture>(texture); });  // TODO: remove _VARIADIC_MAX 6 in MSVC12 with compatible boost
        
        return mTextures.back()->mTextureID;
    }


    void OpenGLRenderer::DrawRenderables(const RenderQueue& renderQueue, const RenderableLighting& lighting, const DebugOptions::RenderingMode debugRenderering)
    {
        // clear the default FBO
        GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        GeometryPass(renderQueue);

        if (debugRenderering == DebugOptions::RENDER_DEBUG_NONE)
            ShadingPass();
        else
            DrawGBufferToScreen(debugRenderering);
    }


    float OpenGLRenderer::GetMaxAnisotropicFiltering() const
    {
        float maxAniso = 0.0f;
        GLCALL(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso));

        return maxAniso;
    }

    float OpenGLRenderer::GetCurrentAnisotropicFiltering() const
    {
        return mCurrentAnisotropy;
    }
        
    bool OpenGLRenderer::SetAnisotropicFiltering(const float newAnisoLevel)
    {
        if (newAnisoLevel < 0.0f || newAnisoLevel > GetMaxAnisotropicFiltering())
            return false;

        mCurrentAnisotropy = newAnisoLevel;
        GLCALL(glSamplerParameterf(mTextureSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, mCurrentAnisotropy));

        return true;
    }


    float OpenGLRenderer::GetZNear() const
    { 
        return Z_NEAR; 
    }
    
    float OpenGLRenderer::GetZFar() const
    { 
        return Z_FAR;
    }


    void OpenGLRenderer::GeometryPass(const RenderQueue& renderQueue)
    {
        GLCALL(glUseProgram(mGeometryProgram.mProgramHandle));
        GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBuffer.mFramebuffer));
        // clear GBuffer fbo
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
                BindTexture2D(OpenGLTexture::TEXTURE_UNIT_DIFFUSE, renderable.mDiffuseTexture, mTextures, mLogger);

            if (hasNormalTexture)
                BindTexture2D(OpenGLTexture::TEXTURE_UNIT_NORMAL, renderable.mNormalTexture, mTextures, mLogger);

            GLCALL(glBindVertexArray(meshIterator->second));
            GLCALL(glDrawElements(GL_TRIANGLES, meshIterator->first->mIndices, GL_UNSIGNED_INT, 0));
            GLCALL(glBindVertexArray(0));

            if (hasDiffuseTexture)
                UnbindTexture2D(OpenGLTexture::TEXTURE_UNIT_DIFFUSE, mLogger);

            if (hasNormalTexture)
                UnbindTexture2D(OpenGLTexture::TEXTURE_UNIT_NORMAL, mLogger);
        }

        GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
        GLCALL(glUseProgram(0));
    }
        
    void OpenGLRenderer::ShadingPass()
    {
    }

    void OpenGLRenderer::DrawGBufferToScreen(const DebugOptions::RenderingMode debugOptions)
    {
        GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBuffer.mFramebuffer));
        switch (debugOptions)
        {
            case DebugOptions::RENDER_DEBUG_POSITIONS:
            {
                GLCALL(glReadBuffer(GL_COLOR_ATTACHMENT0 + GBuffer::GBUFFER_TEXTURE_POSITION));
                GLCALL(glBlitFramebuffer(0, 0, mWindowWidth, mWindowHeight, 0, 0, mWindowWidth, mWindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR));
                break;
            }
            case DebugOptions::RENDER_DEBUG_NORMALS:
            {
                GLCALL(glReadBuffer(GL_COLOR_ATTACHMENT0 + GBuffer::GBUFFER_TEXTURE_NORMAL));
                GLCALL(glBlitFramebuffer(0, 0, mWindowWidth, mWindowHeight, 0, 0, mWindowWidth, mWindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR));
                break;
            }
            case DebugOptions::RENDER_DEBUG_TEXCOORDS:
            {
                GLCALL(glReadBuffer(GL_COLOR_ATTACHMENT0 + GBuffer::GBUFFER_TEXTURE_TEXCOORD));
                GLCALL(glBlitFramebuffer(0, 0, mWindowWidth, mWindowHeight, 0, 0, mWindowWidth, mWindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR));
                break;
            }
            case DebugOptions::RENDER_DEBUG_COLOR:
            {
                GLCALL(glReadBuffer(GL_COLOR_ATTACHMENT0 + GBuffer::GBUFFER_TEXTURE_DIFFUSE));
                GLCALL(glBlitFramebuffer(0, 0, mWindowWidth, mWindowHeight, 0, 0, mWindowWidth, mWindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR));
                break;
            }
            default:
                break;
        }

        GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
    }
}