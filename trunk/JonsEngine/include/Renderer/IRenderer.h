#pragma once

#include "include/Renderer/IVertexBuffer.h"
#include "include/Renderer/RenderItem.h"
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
        virtual void DrawRenderables(const std::vector<RenderItem>& renderQueue) = 0;
        
        virtual RenderBackendType GetRenderBackendType() const = 0;
    };
}