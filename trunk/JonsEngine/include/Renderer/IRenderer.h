#pragma once

#include "include/Renderer/ITexture.h"
#include "include/Renderer/IMesh.h"
#include "include/Renderer/Renderable.h"
#include "include/Renderer/RenderableLighting.h"
#include "include/Core/EngineDefs.h"

#include <vector>
#include <memory>

namespace JonsEngine
{
    class IRenderer;
    class IMemoryAllocator;

    /* RendererRefPtr definition */
    typedef std::unique_ptr<IRenderer, void(*)(IRenderer* renderer)> ManagedRenderer;
    typedef ManagedRenderer& RendererRefPtr;

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

        virtual MeshPtr CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<uint32_t>& indexData) = 0;
        virtual TexturePtr CreateTexture(ITexture::TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ITexture::TextureFormat textureFormat) = 0;

        virtual void DrawRenderables(const RenderQueue& renderQueue, const RenderableLighting& lighting) = 0;
        virtual RenderBackendType GetRenderBackendType() const = 0;

        virtual float GetMaxAnisotropicFiltering() const = 0;
        virtual float GetCurrentAnisotropicFiltering() const = 0;
        virtual bool SetAnisotropicFiltering(const float newAnisoLevel) = 0;

        virtual float GetZNear() const = 0;
        virtual float GetZFar() const = 0;
    };
}