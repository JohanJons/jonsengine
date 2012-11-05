#include "include/Video/Renderer.h"

#include "include/Video/DummyRenderBackend.h"
#include "include/Video/OpenGL/OpenGLRenderBackend.h"
#include "include/Core/EngineSettings.h"
#include "include/Core/Memory/IMemoryAllocator.h"

namespace JonsEngine
{
    Renderer::Renderer(const EngineSettings& engineSettings)  : mMemoryAllocator(Globals::GetDefaultHeapAllocator()), mRenderBackend(CreateBackend(engineSettings))
    {
    }
        
    Renderer::~Renderer()
    {
        mMemoryAllocator.DeallocateObject(mRenderBackend);
    }

    void Renderer::StartFrame()                                                                                         { mRenderBackend->StartFrame();  }
    void Renderer::EndFrame()                                                                                           { mRenderBackend->EndFrame();    }

    bool Renderer::SetupWindow(const ScreenMode& screenMode)                                                            { return mRenderBackend->SetupWindow(screenMode);   }
    void Renderer::CloseWindow()                                                                                        { mRenderBackend->CloseWindow();                    }
    bool Renderer::IsWindowOpened() const                                                                               { return mRenderBackend->IsWindowOpened();          }
    uint16_t Renderer::GetCurrentFPS() const                                                                            { return mRenderBackend->GetCurrentFPS();           }

    void Renderer::SetFullscreen(bool fullscreen)                                                                       { mRenderBackend->SetFullscreen(fullscreen);            }
    void Renderer::SetScreenResolution(const uint16_t width, const uint16_t height)                                     { mRenderBackend->SetScreenResolution(width, height);   }
    void Renderer::SetWindowTitle(const std::string& windowTitle)                                                       { mRenderBackend->SetWindowTitle(windowTitle);          }

    void Renderer::RenderVertexArrays()                                                                                 { mRenderBackend->RenderVertexArrays();                             }
    void Renderer::DrawLine(const Vec3& pointA, const Vec3& pointB)                                                     { mRenderBackend->DrawLine(pointA, pointB);                         }
    void Renderer::DrawTriangle(const Vec3& pointA, const Vec3& pointB, const Vec3& pointC)                             { mRenderBackend->DrawTriangle(pointA, pointB, pointC);             }
    void Renderer::DrawRectangle(const Vec3& pointA, const Vec3& pointB, const Vec3& pointC, const Vec3& pointD)        { mRenderBackend->DrawRectangle(pointA, pointB, pointC, pointD);    }

    const ScreenMode& Renderer::GetScreenMode() const                                                                   { return mRenderBackend->GetScreenMode();  }
    const std::string& Renderer::GetWindowTitle() const                                                                 { return mRenderBackend->GetWindowTitle(); }

    RenderBackend* Renderer::CreateBackend(const EngineSettings& engineSettings)
    {
        switch (engineSettings.RenderBackend)
        {
            case RenderBackend::OPENGL:
                return mMemoryAllocator.AllocateObject<OpenGLRenderBackend>(engineSettings);
                break;

            default:
                return mMemoryAllocator.AllocateObject<DummyRenderBackend>();
                break;

        }
    }
}