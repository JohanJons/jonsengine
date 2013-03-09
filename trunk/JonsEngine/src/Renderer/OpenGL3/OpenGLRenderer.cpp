#include "include/Renderer/OpenGL3/OpenGLRenderer.h"

#include "include/Renderer/OpenGL3/OpenGLVertexBuffer.h"
#include "include/Renderer/OpenGL3/Shader.h"
#include "include/Core/EngineSettings.h"
#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/Utils/Math.h"

#include "boost/bind.hpp"
#include "boost/foreach.hpp"
#include "GL/glew.h"
#include "GL/glfw.h"
#include <glm/gtc/type_ptr.hpp>
#include <exception>


namespace JonsEngine
{
    OpenGLRenderer::OpenGLRenderer(const EngineSettings& engineSettings) : mLogger(Logger::GetRendererLogger()), mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator()), mDefaultProgram("DefaultProgram"), mUniBuffer("DefaultUniform")
    {
        // 'glewExperimental = GL_TRUE' needed to initialize openGL core profile; see GLEW doc
        glewExperimental = GL_TRUE;
        GLenum glewErr = glewInit();

        if (!GLEW_VERSION_3_3)
        {
            JONS_LOG_ERROR(mLogger, "OpenGLRenderer::OpenGLRenderer(): Minimum OpenGL driver (OpenGL 3.3) not supported!");
            throw std::runtime_error("OpenGLRenderer::OpenGLRenderer(): Minimum OpenGL driver (OpenGL 3.3) not supported!");
        }

        // face culling - TODO
        //glEnable(GL_CULL_FACE);
	    //glCullFace(GL_BACK);
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

    VertexBufferPtr OpenGLRenderer::CreateVertexBuffer(const std::vector<float>& vertexData, const std::vector<uint32_t>& indexData)
    {
        return CreateVertexBuffer(&vertexData[0], vertexData.size(), &indexData[0], indexData.size());
    }

    VertexBufferPtr OpenGLRenderer::CreateVertexBuffer(const float vertexData[], const size_t vertexDataSize, const uint32_t indexData[], const size_t indexDataSize)
    {
        return VertexBufferPtr(HeapAllocator::GetDefaultHeapAllocator().AllocateObject<OpenGLVertexBuffer>(vertexData, vertexDataSize, indexData, indexDataSize), boost::bind(&HeapAllocator::DeallocateObject<OpenGLVertexBuffer>, &HeapAllocator::GetDefaultHeapAllocator(), _1));
    }


    void OpenGLRenderer::BeginRendering()
    {
        glClearDepth(1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRenderer::DrawRenderables(const std::vector<RenderItem>& renderQueue)
    {
        BOOST_FOREACH(const RenderItem& renderItem, renderQueue)
        {
            ShaderData shaderData;
            shaderData.Color.x = 1.0f; shaderData.Color.y = 0.5f; shaderData.Color.z = 0.5f; shaderData.Color.w = 1.0f;
            shaderData.MVPMatrix = renderItem.mMVP;
            mUniBuffer.SetData(shaderData);

            renderItem.mVertexBuffer->Render();
        }
    }

    void OpenGLRenderer::EndRendering()
    {
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

            mDefaultProgram.BindAttribLocation(0, "in_position");

            mDefaultProgram.LinkProgram();

            if (mDefaultProgram.IsLinked())
                mDefaultProgram.UseProgram(true);

            mDefaultProgram.UseUniform(mUniBuffer, true);
        }
      
    }
}