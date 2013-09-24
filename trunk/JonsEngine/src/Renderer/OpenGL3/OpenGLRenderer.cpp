#include "include/Renderer/OpenGL3/OpenGLRenderer.h"

#include "include/Renderer/OpenGL3/OpenGLMesh.h"
#include "include/Renderer/OpenGL3/OpenGLTexture.h"
#include "include/Renderer/OpenGL3/Shader.h"
#include "include/Renderer/OpenGL3/Shaders.h"
#include "include/Renderer/OpenGL3/OpenGLUtils.hpp"
#include "include/Core/EngineSettings.h"
#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"

#include "boost/bind.hpp"
#include "boost/foreach.hpp"
#include "GL/glew.h"
#include <exception>


namespace JonsEngine
{
    OpenGLRenderer::OpenGLRenderer(const EngineSettings& engineSettings) : OpenGLRenderer(engineSettings.mAnisotropicFiltering)
    {

    }

    OpenGLRenderer::OpenGLRenderer(const float anisotropicLevel) : mLogger(Logger::GetRendererLogger()), mDefaultProgram("DefaultProgram", gVertexShader, gFragmentShader), 
                                                                           mUniBufferTransform("UnifTransform", mDefaultProgram), mUniBufferMaterial("UnifMaterial", mDefaultProgram), 
                                                                           mUniBufferLightingInfo("UnifLighting", mDefaultProgram), mCurrentAnisotropy(anisotropicLevel), mFirstPass(true)
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

    MeshPtr OpenGLRenderer::CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<uint32_t>& indexData)
    {
        return MeshPtr(HeapAllocator::GetDefaultHeapAllocator().AllocateObject<OpenGLMesh>(vertexData, normalData, texCoords, indexData), boost::bind(&HeapAllocator::DeallocateObject<OpenGLMesh>, &HeapAllocator::GetDefaultHeapAllocator(), _1));
    }

    TexturePtr OpenGLRenderer::CreateTexture(ITexture::TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ITexture::TextureFormat textureFormat)
    {
        return TexturePtr(HeapAllocator::GetDefaultHeapAllocator().AllocateObject<OpenGLTexture>(textureData, textureWidth, textureHeight, textureFormat), 
                          boost::bind(&HeapAllocator::DeallocateObject<OpenGLTexture>, &HeapAllocator::GetDefaultHeapAllocator(), _1));
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

        int activeIndex = 0;
        BOOST_FOREACH(const Renderable& renderable, renderQueue)
        {
            if (renderable.mMesh)
            {
                mUniBufferTransform.SetData(Transform(renderable.mWVPMatrix, renderable.mWorldMatrix, renderable.mTextureTilingFactor));
                mUniBufferMaterial.SetData(Material(renderable.mDiffuseColor, renderable.mAmbientColor, renderable.mSpecularColor, renderable.mEmissiveColor,
                                                    renderable.mDiffuseTexture != NULL, renderable.mLightingEnabled, renderable.mSpecularFactor));

                if (renderable.mDiffuseTexture)
                {
                    glActiveTexture(OpenGLTexture::TEXTURE_UNIT_DIFFUSE);
                    CHECK_GL_ERROR(mLogger);
                    OpenGLTexture* diffuseTexture = static_cast<OpenGLTexture*>(renderable.mDiffuseTexture.get());
                    glBindTexture(GL_TEXTURE_2D, diffuseTexture->mTexture);
                    CHECK_GL_ERROR(mLogger);
                }

                renderable.mMesh->Render();
            }

            activeIndex++;
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
}