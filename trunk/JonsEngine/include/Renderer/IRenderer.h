#pragma once

#include "include/Renderer/IVertexBuffer.h"
#include "include/Renderer/Renderable.h"
#include "include/Renderer/RenderableLighting.h"
#include "include/Core/EngineDefs.h"

#include <vector>

namespace JonsEngine
{

    /* IRenderer definition */
    class IRenderer
    {
    public:
        enum RenderBackendType
        {
            OPENGL = 0,
            NONE
        };

        virtual ~IRenderer() { }

        virtual VertexBufferPtr CreateVertexBuffer(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<uint32_t>& indexData) = 0;
        virtual void DrawRenderables(const RenderQueue& renderQueue, const RenderableLighting& lighting) = 0;
        
        virtual RenderBackendType GetRenderBackendType() const = 0;
    };
}