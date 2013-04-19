#include "include/Renderer/OpenGL3/OpenGLRenderer.h"

#include "include/Renderer/OpenGL3/OpenGLVertexBuffer.h"
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
                                                                            mUniBufferTransform("UniTransform", mDefaultProgram), mUniBufferMaterial("UniMaterial", mDefaultProgram), 
                                                                            mUniBufferLightingInfo("UniLightingInfo", mDefaultProgram),  mUniBufferLights("UniLights", mDefaultProgram)
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
    }
        
    OpenGLRenderer::~OpenGLRenderer()
    {
    }

    VertexBufferPtr OpenGLRenderer::CreateVertexBuffer(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<uint32_t>& indexData)
    {
        return VertexBufferPtr(HeapAllocator::GetDefaultHeapAllocator().AllocateObject<OpenGLVertexBuffer>(vertexData, normalData, indexData), boost::bind(&HeapAllocator::DeallocateObject<OpenGLVertexBuffer>, &HeapAllocator::GetDefaultHeapAllocator(), _1));
    }


    void OpenGLRenderer::DrawRenderables(const RenderQueue& renderQueue, const RenderableLighting& lighting)
    {
        glClearDepth(1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // set active lights
        mUniBufferLights.AddData(lighting.mLights);
        mUniBufferLightingInfo.AddData(lighting.mLightingInfo);

        mUniBufferLights.SetActiveIndex(0);
        mUniBufferLightingInfo.SetActiveIndex(0);

        int activeIndex = 0;
        BOOST_FOREACH(const Renderable& renderable, renderQueue)
        {
            mUniBufferTransform.AddData(renderable.mTransform);
            mUniBufferMaterial.AddData(renderable.mMaterial);

            mUniBufferTransform.SetActiveIndex(activeIndex);
            mUniBufferMaterial.SetActiveIndex(activeIndex);

            renderable.mVertexBuffer->Render();

            activeIndex++;
        }

        mUniBufferLights.ClearData();
        mUniBufferLightingInfo.ClearData();
        mUniBufferTransform.ClearData();
        mUniBufferMaterial.ClearData();
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