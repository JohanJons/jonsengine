#include "include/Renderer/OpenGL3/OpenGLRenderer.h"

#include "include/Renderer/OpenGL3/OpenGLMesh.h"
#include "include/Renderer/OpenGL3/OpenGLTexture.h"
#include "include/Renderer/OpenGL3/Shader.h"
#include "include/Renderer/OpenGL3/Shaders.h"
#include "include/Core/EngineSettings.h"
#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"

#include "boost/bind.hpp"
#include "boost/foreach.hpp"
#include "GL/glew.h"
#include <exception>


namespace JonsEngine
{
    OpenGLRenderer::OpenGLRenderer(const EngineSettings& engineSettings) : mLogger(Logger::GetRendererLogger()), mDefaultProgram(SetupShaderProgram("DefaultProgram")), 
                                                                           mUniBufferTransform("UnifTransform", mDefaultProgram), mUniBufferMaterial("UnifMaterial", mDefaultProgram), 
                                                                           mUniBufferLightingInfo("UnifLighting", mDefaultProgram)
    {
        // 'glewExperimental = GL_TRUE' needed to initialize openGL core profile; see GLEW doc
        glewExperimental = GL_TRUE;
        GLenum glewErr = glewInit();

        if (!GLEW_VERSION_3_3)
        {
            JONS_LOG_ERROR(mLogger, "OpenGLRenderer::OpenGLRenderer(): Minimum OpenGL driver (OpenGL 3.3) not supported!");
            throw std::runtime_error("OpenGLRenderer::OpenGLRenderer(): Minimum OpenGL driver (OpenGL 3.3) not supported!");
        }

        // face culling
        glEnable(GL_CULL_FACE);
	    glCullFace(GL_BACK);
	    glFrontFace(GL_CCW);

        // z depth testing
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LEQUAL);
        glDepthRange(0.0f, 1.0f);

        // texture sampler setup
        glGenSamplers(1, &mTextureSampler);
        glSamplerParameteri(mTextureSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(mTextureSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glUniform1i(glGetUniformLocation(mDefaultProgram.GetHandle(), "unifDiffuseTexture"), OpenGLTexture::TEXTURE_UNIT_DIFFUSE);
        glBindSampler(OpenGLTexture::TEXTURE_UNIT_DIFFUSE, mTextureSampler);
    }
        
    OpenGLRenderer::~OpenGLRenderer()
    {
        glDeleteSamplers(1, &mTextureSampler);
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
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // set active lights
        mUniBufferLightingInfo.SetData(lighting);

        int activeIndex = 0;
        BOOST_FOREACH(const Renderable& renderable, renderQueue)
        {
            if (renderable.mMesh)
            {
                mUniBufferTransform.SetData(Transform(renderable.mWVPMatrix, renderable.mWorldMatrix));
                mUniBufferMaterial.SetData(Material(renderable.mDiffuseColor, renderable.mAmbientColor, renderable.mSpecularColor, renderable.mEmissiveColor,
                                                    renderable.mDiffuseTexture != NULL, renderable.mLightingEnabled, renderable.mSpecularFactor));

                if (renderable.mDiffuseTexture)
                {
                    glActiveTexture(OpenGLTexture::TEXTURE_UNIT_DIFFUSE);
                    OpenGLTexture* diffuseTexture = static_cast<OpenGLTexture*>(renderable.mDiffuseTexture.get());
                    glBindTexture(GL_TEXTURE_2D, diffuseTexture->mTexture);
                }

                renderable.mMesh->Render();
            }

            activeIndex++;
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }


    ShaderProgram OpenGLRenderer::SetupShaderProgram(const std::string& programName)
    {
        ShaderProgram ret(programName);
        
        Shader vertexShader("VertexShader", Shader::VERTEX_SHADER);
        Shader fragmentShader("FragmentShader", Shader::FRAGMENT_SHADER);

        vertexShader.Compile(gVertexShader);
        fragmentShader.Compile(gFragmentShader);

        if (vertexShader.IsCompiled() && fragmentShader.IsCompiled())
        {
            ret.AddShader(&vertexShader);
            ret.AddShader(&fragmentShader);

            ret.LinkProgram();

            if (ret.IsLinked())
                ret.UseProgram(true);
            else
            {
                JONS_LOG_ERROR(mLogger, "OpenGLRenderer::SetupShaders(): Failed to link program!");
                throw std::runtime_error("OpenGLRenderer::SetupShaders(): Failed to link program!");
            }
        }
        else
        {
            JONS_LOG_ERROR(mLogger, "OpenGLRenderer::SetupShaders(): Failed to compile shaders!");
            throw std::runtime_error("OpenGLRenderer::SetupShaders(): Failed to compile shaders!");
        }

        return ret;
    }
}