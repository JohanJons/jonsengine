#pragma once

#include "include/Renderer/IVertexBuffer.h"
#include "include/Renderer/IMaterial.h"
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

        virtual VertexBufferPtr CreateVertexBuffer(const std::vector<Vec3>& vertexData, const std::vector<Vec3>& normalData, const std::vector<Vec2>& texCoords, const std::vector<uint32_t>& indexData) = 0;
        virtual MaterialPtr CreateMaterial() = 0;
        virtual void DrawRenderables(const RenderQueue& renderQueue, const RenderableLighting& lighting) = 0;
        
        virtual RenderBackendType GetRenderBackendType() const = 0;
    };
}