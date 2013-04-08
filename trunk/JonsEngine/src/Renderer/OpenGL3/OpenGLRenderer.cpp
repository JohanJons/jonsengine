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
    OpenGLRenderer::OpenGLRenderer(const EngineSettings& engineSettings) : mLogger(Logger::GetRendererLogger()), mDefaultProgram("DefaultProgram"), mUniBufferTransform("Transform"), mUniBufferLight("Light")
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

        SetupShaders();
    }
        
    OpenGLRenderer::~OpenGLRenderer()
    {
    }

    VertexBufferPtr OpenGLRenderer::CreateVertexBuffer(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<uint32_t>& indexData)
    {
        return VertexBufferPtr(HeapAllocator::GetDefaultHeapAllocator().AllocateObject<OpenGLVertexBuffer>(vertexData, normalData, indexData), boost::bind(&HeapAllocator::DeallocateObject<OpenGLVertexBuffer>, &HeapAllocator::GetDefaultHeapAllocator(), _1));
    }


    void OpenGLRenderer::DrawRenderables(const std::vector<RenderItem>& renderQueue)
    {
        glClearDepth(1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        BOOST_FOREACH(const RenderItem& renderItem, renderQueue)
        {
            std::vector<float> transformData(renderItem.CopyTransformData());
            std::vector<float> lightData(renderItem.CopyLightData());

            mUniBufferTransform.SetData(transformData);
            mUniBufferLight.SetData(lightData);

            renderItem.mVertexBuffer->Render();
        }
    }


    void OpenGLRenderer::SetupShaders()
    {
        Shader vertexShader("VertexShader", Shader::VERTEX_SHADER);
        Shader fragmentShader("FragmentShader", Shader::FRAGMENT_SHADER);

        vertexShader.Compile(gVertexShader);
        fragmentShader.Compile(gFragmentShader);

        if (vertexShader.IsCompiled() && fragmentShader.IsCompiled())
        {
            mDefaultProgram.AddShader(&vertexShader);
            mDefaultProgram.AddShader(&fragmentShader);

            mDefaultProgram.LinkProgram();

            if (mDefaultProgram.IsLinked())
                mDefaultProgram.UseProgram(true);

            mDefaultProgram.UseUniform(mUniBufferTransform, true);
            mDefaultProgram.UseUniform(mUniBufferLight, true);
        }
        else
        {
            JONS_LOG_ERROR(mLogger, "OpenGLRenderer::SetupShaders(): Failed to compile shaders!");
            throw std::runtime_error("OpenGLRenderer::SetupShaders(): Failed to compile shaders!");
        }
    }
}