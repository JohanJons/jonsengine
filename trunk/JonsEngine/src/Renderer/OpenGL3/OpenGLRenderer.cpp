#include "include/Renderer/OpenGL3/OpenGLRenderer.h"

#include "include/Renderer/OpenGL3/OpenGLMesh.h"
#include "include/Renderer/OpenGL3/OpenGLTexture.h"
#include "include/Renderer/OpenGL3/Shader.h"
#include "include/Renderer/OpenGL3/Shaders.h"
#include "include/Renderer/OpenGL3/OpenGLUtils.hpp"
#include "include/Core/EngineSettings.h"
#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"

#include "GL/glew.h"
#include <exception>
#include <functional>
#include <utility>
#include <exception>

namespace JonsEngine
{
    void BindTexture(const OpenGLTexture::TextureUnit textureUnit, const TextureID targetTexture, const std::vector<OpenGLTexturePtr>& textures, Logger& logger)
    {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        CHECK_GL_ERROR(logger);

        // TODO: sort textures
        auto texture = std::find_if(textures.begin(), textures.end(), [&](const OpenGLTexturePtr ptr) { return ptr->mTextureID == targetTexture; });
        if (texture == textures.end())
        {
            JONS_LOG_ERROR(logger, "Renderable TextureID out of range");
            throw std::runtime_error("Renderable TextureID out of range");
        }

        glBindTexture(GL_TEXTURE_2D, texture->get()->mTextureID);
        CHECK_GL_ERROR(logger);
    }

    void UnbindTexture(const OpenGLTexture::TextureUnit textureUnit, Logger& logger)
    {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        CHECK_GL_ERROR(logger);
        glBindTexture(GL_TEXTURE_2D, 0);
        CHECK_GL_ERROR(logger);
    }


    OpenGLRenderer::OpenGLRenderer(const EngineSettings& engineSettings, IMemoryAllocatorPtr memoryAllocator) : OpenGLRenderer(engineSettings.mAnisotropicFiltering, memoryAllocator)
    {
    }

    OpenGLRenderer::OpenGLRenderer(OpenGLRenderer& renderer, IMemoryAllocatorPtr memoryAllocator) : OpenGLRenderer(renderer.mCurrentAnisotropy, memoryAllocator)
    {
        // move the meshes, recreate the VAO
        for (const OpenGLMeshPair mesh : renderer.mMeshes)
        {
            glDeleteVertexArrays(1, &mesh.second);
            CHECK_GL_ERROR(mLogger);

            mMeshes.emplace_back(mesh.first, SetupVAO(mesh.first));
        }
        renderer.mMeshes.clear();

        for (const OpenGLTexturePtr texture : renderer.mTextures)
            mTextures.push_back(texture);
    }

    OpenGLRenderer::OpenGLRenderer(const float anisotropy, IMemoryAllocatorPtr memoryAllocator) : 
        mMemoryAllocator(memoryAllocator), mLogger(Logger::GetRendererLogger()),
        mDefaultProgram("DefaultProgram", gVertexShader, gFragmentShader), mUniBufferTransform("UnifTransform", mLogger, mDefaultProgram), mUniBufferMaterial("UnifMaterial", mLogger, mDefaultProgram), 
        mUniBufferLightingInfo("UnifLighting", mLogger, mDefaultProgram), mTextureSampler(0), mCurrentAnisotropy(anisotropy)
    {
        // face culling
        glEnable(GL_CULL_FACE);
        CHECK_GL_ERROR(mLogger);
	    glCullFace(GL_BACK);
        CHECK_GL_ERROR(mLogger);
	    glFrontFace(GL_CCW);
        CHECK_GL_ERROR(mLogger);

        // z depth testing
        glEnable(GL_DEPTH_TEST);
        CHECK_GL_ERROR(mLogger);
        glDepthMask(GL_TRUE);
        CHECK_GL_ERROR(mLogger);
        glDepthFunc(GL_LEQUAL);
        CHECK_GL_ERROR(mLogger);
        glDepthRange(0.0f, 1.0f);
        CHECK_GL_ERROR(mLogger);

        // texture sampler setup
        glGenSamplers(1, &mTextureSampler);
        CHECK_GL_ERROR(mLogger);
        glSamplerParameteri(mTextureSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECK_GL_ERROR(mLogger);
        glSamplerParameteri(mTextureSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        CHECK_GL_ERROR(mLogger);
        glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
        CHECK_GL_ERROR(mLogger);
        glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
        CHECK_GL_ERROR(mLogger);
        glSamplerParameterf(mTextureSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, mCurrentAnisotropy);
        CHECK_GL_ERROR(mLogger);

        glUniform1i(glGetUniformLocation(mDefaultProgram.GetHandle(), "unifDiffuseTexture"), OpenGLTexture::TEXTURE_UNIT_DIFFUSE);
        CHECK_GL_ERROR(mLogger);
        glUniform1i(glGetUniformLocation(mDefaultProgram.GetHandle(), "unifNormalTexture"), OpenGLTexture::TEXTURE_UNIT_NORMAL);
        CHECK_GL_ERROR(mLogger);
        glBindSampler(OpenGLTexture::TEXTURE_UNIT_DIFFUSE, mTextureSampler);
        CHECK_GL_ERROR(mLogger);
        glBindSampler(OpenGLTexture::TEXTURE_UNIT_NORMAL, mTextureSampler);
        CHECK_GL_ERROR(mLogger);

        SetAnisotropicFiltering(mCurrentAnisotropy);
    }

    OpenGLRenderer::~OpenGLRenderer()
    {
        for (const OpenGLMeshPair mesh : mMeshes)
        {
            glDeleteVertexArrays(1, &mesh.second);
            CHECK_GL_ERROR(mLogger);
        }

        glDeleteSamplers(1, &mTextureSampler);
        CHECK_GL_ERROR(mLogger);
    }


    MeshID OpenGLRenderer::CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData)
    {
        auto allocator = mMemoryAllocator;
        OpenGLMeshPtr meshPtr(allocator->AllocateObject<OpenGLMesh>(vertexData, normalData, texCoords, tangents, bitangents, indexData, mLogger), [=](OpenGLMesh* mesh) { allocator->DeallocateObject<OpenGLMesh>(mesh); });

        mMeshes.emplace_back(meshPtr, SetupVAO(meshPtr));

        return mMeshes.back().first->mMeshID;
    }

    TextureID OpenGLRenderer::CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, TextureFormat textureFormat)
    {
        auto allocator = mMemoryAllocator;
        mTextures.emplace_back(allocator->AllocateObject<OpenGLTexture>(textureData, textureWidth, textureHeight, textureFormat, textureType, mLogger), [=](OpenGLTexture* texture) { allocator->DeallocateObject<OpenGLTexture>(texture); });  // TODO: remove _VARIADIC_MAX 6 in MSVC12 with compatible boost
        
        return mTextures.back()->mTextureID;
    }


    void OpenGLRenderer::DrawRenderables(const RenderQueue& renderQueue, const RenderableLighting& lighting)
    {
        glClearDepth(1.0f);
        CHECK_GL_ERROR(mLogger);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        CHECK_GL_ERROR(mLogger);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        CHECK_GL_ERROR(mLogger);

        // set active lights
        mUniBufferLightingInfo.SetData(lighting);

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

            mUniBufferTransform.SetData(Transform(renderable.mWVPMatrix, renderable.mWorldMatrix, renderable.mTextureTilingFactor));
            mUniBufferMaterial.SetData(Material(renderable.mDiffuseColor, renderable.mAmbientColor, renderable.mSpecularColor, renderable.mEmissiveColor,
                                                renderable.mDiffuseTexture != INVALID_TEXTURE_ID, renderable.mNormalTexture != INVALID_TEXTURE_ID, renderable.mLightingEnabled, renderable.mSpecularFactor));

            if (renderable.mDiffuseTexture != INVALID_TEXTURE_ID)
                BindTexture(OpenGLTexture::TEXTURE_UNIT_DIFFUSE, renderable.mDiffuseTexture, mTextures, mLogger);

            if (renderable.mNormalTexture != INVALID_TEXTURE_ID)
                BindTexture(OpenGLTexture::TEXTURE_UNIT_NORMAL, renderable.mNormalTexture, mTextures, mLogger);

            glBindVertexArray(meshIterator->second);
            CHECK_GL_ERROR(mLogger);
            glDrawElements(GL_TRIANGLES, meshIterator->first->mIndices, GL_UNSIGNED_INT, 0);
            CHECK_GL_ERROR(mLogger);
            glBindVertexArray(0);
            CHECK_GL_ERROR(mLogger);

            if (renderable.mDiffuseTexture != INVALID_TEXTURE_ID)
                UnbindTexture(OpenGLTexture::TEXTURE_UNIT_DIFFUSE, mLogger);

            if (renderable.mNormalTexture != INVALID_TEXTURE_ID)
                UnbindTexture(OpenGLTexture::TEXTURE_UNIT_NORMAL, mLogger);
        }

    }


    float OpenGLRenderer::GetMaxAnisotropicFiltering() const
    {
        float maxAniso = 0.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
        CHECK_GL_ERROR(mLogger);

        return maxAniso;
    }

    float OpenGLRenderer::GetCurrentAnisotropicFiltering() const
    {
        return mCurrentAnisotropy;
    }
        
    bool OpenGLRenderer::SetAnisotropicFiltering(const float newAnisoLevel)
    {
        if (newAnisoLevel <= 0.0f || newAnisoLevel > GetMaxAnisotropicFiltering())
            return false;

        mCurrentAnisotropy = newAnisoLevel;
        glSamplerParameterf(mTextureSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, mCurrentAnisotropy);
        CHECK_GL_ERROR(mLogger);
        
        return true;
    }

    OpenGLRenderer::VAO OpenGLRenderer::SetupVAO(OpenGLMeshPtr mesh)
    {
        VAO vao;

        glGenVertexArrays(1, &vao);
        CHECK_GL_ERROR(mLogger);

        glBindVertexArray(vao);
        CHECK_GL_ERROR(mLogger);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->mVBO);
        CHECK_GL_ERROR(mLogger);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->mIndexBuffer);
        CHECK_GL_ERROR(mLogger);

        glEnableVertexAttribArray(0);
        CHECK_GL_ERROR(mLogger);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        CHECK_GL_ERROR(mLogger);
        glEnableVertexAttribArray(1);
        CHECK_GL_ERROR(mLogger);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(mesh->mVertexDataSize));
        CHECK_GL_ERROR(mLogger);
        glEnableVertexAttribArray(2);
        CHECK_GL_ERROR(mLogger);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(mesh->mVertexDataSize + mesh->mNormalDataSize));
        CHECK_GL_ERROR(mLogger);
        glEnableVertexAttribArray(3);
        CHECK_GL_ERROR(mLogger);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(mesh->mVertexDataSize + mesh->mNormalDataSize + mesh->mTexCoordsSize));
        CHECK_GL_ERROR(mLogger);
        glEnableVertexAttribArray(4);
        CHECK_GL_ERROR(mLogger);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(mesh->mVertexDataSize + mesh->mNormalDataSize + mesh->mTexCoordsSize + mesh->mTangentsSize));
        CHECK_GL_ERROR(mLogger);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CHECK_GL_ERROR(mLogger);
        glBindVertexArray(0);
        CHECK_GL_ERROR(mLogger);
        
        return vao;
    }
}