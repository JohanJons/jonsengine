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


namespace JonsEngine
{
    MeshID gNextMeshID       = 0;
    TextureID gNextTextureID = 0;


    OpenGLRenderer::OpenGLRenderer(const EngineSettings& engineSettings, IMemoryAllocator& memoryAllocator) : OpenGLRenderer(engineSettings.mAnisotropicFiltering, memoryAllocator)
    {
    }

    OpenGLRenderer::OpenGLRenderer(const OpenGLRenderer& renderer) : OpenGLRenderer(renderer.mCurrentAnisotropy, renderer.mMemoryAllocator)
    {
        mMeshes   = renderer.mMeshes;
        mTextures = renderer.mTextures;

        for (OpenGLMesh& mesh : *mMeshes)
            SetupVAO(mesh);
    }

    OpenGLRenderer::OpenGLRenderer(const float anisotropy, IMemoryAllocator& memoryAllocator) : 
        mMemoryAllocator(memoryAllocator),
        mMeshes(HeapAllocator::GetDefaultHeapAllocator().AllocateObject<std::vector<OpenGLMesh>>(), std::bind(&HeapAllocator::DeallocateObject<std::vector<OpenGLMesh>>, &HeapAllocator::GetDefaultHeapAllocator(), std::placeholders::_1)),
        mTextures(HeapAllocator::GetDefaultHeapAllocator().AllocateObject<std::vector<OpenGLTexture>>(), std::bind(&HeapAllocator::DeallocateObject<std::vector<OpenGLTexture>>, &HeapAllocator::GetDefaultHeapAllocator(), std::placeholders::_1)),
        mLogger(Logger::GetRendererLogger()),
        mDefaultProgram("DefaultProgram", gVertexShader, gFragmentShader), mUniBufferTransform("UnifTransform", mDefaultProgram), mUniBufferMaterial("UnifMaterial", mDefaultProgram), 
        mUniBufferLightingInfo("UnifLighting", mDefaultProgram), mCurrentAnisotropy(anisotropy)
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
        glBindSampler(OpenGLTexture::TEXTURE_UNIT_DIFFUSE, mTextureSampler);
        CHECK_GL_ERROR(mLogger);

        SetAnisotropicFiltering(mCurrentAnisotropy);
    }

    OpenGLRenderer::~OpenGLRenderer()
    {
        glDeleteSamplers(1, &mTextureSampler);
        CHECK_GL_ERROR(mLogger);
    }

    MeshID OpenGLRenderer::CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<uint32_t>& indexData)
    {
        mMeshes->emplace_back(gNextMeshID, vertexData, normalData, texCoords, indexData);

        SetupVAO(mMeshes->at(gNextMeshID));
        
        return gNextMeshID++;
    }

    TextureID OpenGLRenderer::CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, TextureFormat textureFormat)
    {
        mTextures->emplace_back(gNextTextureID, textureData, textureWidth, textureHeight, textureFormat, textureType);  // TODO: remove _VARIADIC_MAX 6 in MSVC12 with compatible boost
        
        return gNextTextureID++;
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

        for (const Renderable& renderable : renderQueue)
        {
            if (renderable.mMesh != INVALID_MESH_ID)
            {
                mUniBufferTransform.SetData(Transform(renderable.mWVPMatrix, renderable.mWorldMatrix, renderable.mTextureTilingFactor));
                mUniBufferMaterial.SetData(Material(renderable.mDiffuseColor, renderable.mAmbientColor, renderable.mSpecularColor, renderable.mEmissiveColor,
                                                    renderable.mDiffuseTexture != NULL, renderable.mLightingEnabled, renderable.mSpecularFactor));

                if (renderable.mDiffuseTexture != INVALID_TEXTURE_ID)
                {
                    glActiveTexture(OpenGLTexture::TEXTURE_UNIT_DIFFUSE);
                    CHECK_GL_ERROR(mLogger);

                    glBindTexture(GL_TEXTURE_2D, mTextures->at(renderable.mDiffuseTexture).mTexture);
                    CHECK_GL_ERROR(mLogger);
                }

                // TODO: bounds check
                OpenGLMesh& mesh = mMeshes->at(renderable.mMesh);
                glBindVertexArray(mesh.mVAO);
                CHECK_GL_ERROR(mLogger);

                glDrawElements(GL_TRIANGLES, mesh.mIndices, GL_UNSIGNED_INT, 0);
                CHECK_GL_ERROR(mLogger);

                glBindVertexArray(0);
                CHECK_GL_ERROR(mLogger);
            }
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        CHECK_GL_ERROR(mLogger);
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


    void OpenGLRenderer::SetupVAO(OpenGLMesh& mesh)
    {
        if (mesh.mVAO)
        {
            glDeleteVertexArrays(1, &mesh.mVAO);
            CHECK_GL_ERROR(mLogger);
        }

        glGenVertexArrays(1, &mesh.mVAO);
        CHECK_GL_ERROR(mLogger);

        glBindVertexArray(mesh.mVAO);
        CHECK_GL_ERROR(mLogger);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.mVBO);
        CHECK_GL_ERROR(mLogger);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.mIndexBuffer);
        CHECK_GL_ERROR(mLogger);

        glEnableVertexAttribArray(0);
        CHECK_GL_ERROR(mLogger);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        CHECK_GL_ERROR(mLogger);
        glEnableVertexAttribArray(1);
        CHECK_GL_ERROR(mLogger);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(mesh.vertexDataSize));
        CHECK_GL_ERROR(mLogger);
        glEnableVertexAttribArray(2);
        CHECK_GL_ERROR(mLogger);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(mesh.vertexDataSize + mesh.normalDataSize));
        CHECK_GL_ERROR(mLogger);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CHECK_GL_ERROR(mLogger);
        glBindVertexArray(0);
        CHECK_GL_ERROR(mLogger);
    }
}