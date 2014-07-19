#pragma once

#include "include/Renderer/RenderCommands.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/DX11Context.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11ConstantBufferDefinitions.h"
#include "include/Core/Types.h"
#include "include/Core/EngineSettings.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/DebugOptions.h"

#include <windows.h>
#include <d3d11.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>


namespace JonsEngine
{
    class Logger;

    class DX11RendererImpl : protected DX11Context
    {
    public:
        typedef std::unique_ptr<DX11Mesh, std::function<void(DX11Mesh*)>> DX11MeshPtr;
        typedef std::unique_ptr<DX11Texture, std::function<void(DX11Texture*)>> DX11TexturePtr;

        DX11RendererImpl(const EngineSettings& settings, Logger& logger, IMemoryAllocatorPtr memoryAllocator);
        ~DX11RendererImpl();

        MeshID CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData);
        TextureID CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ColorFormat colorFormat);
        void Render(const RenderQueue& renderQueue, const RenderableLighting& lighting, const DebugOptions::RenderingMode debugMode, const DebugOptions::RenderingFlags debugExtra);

        EngineSettings::Anisotropic GetAnisotropicFiltering() const;
        void SetAnisotropicFiltering(const EngineSettings::Anisotropic anisotropic);

        EngineSettings::MSAA GetMSAA() const;
        void SetMSAA(const EngineSettings::MSAA msaa);

        float GetZNear() const;
        float GetZFar() const;
        uint32_t GetShadowmapResolution() const;


    private:
        static LRESULT CALLBACK DX11RendererImpl::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
        void SetupContext(const uint32_t viewportWidth, const uint32_t viewportHeight);

        Logger& mLogger;
        IMemoryAllocatorPtr mMemoryAllocator;

        ID3D11Texture2D* mDepthStencilBuffer;
        ID3D11DepthStencilView* mDepthStencilView;
        ID3D11DepthStencilState* mDepthStencilState;
        ID3D11VertexShader* mForwardVertexShader;
        ID3D11PixelShader* mForwardPixelShader;
        ID3D11RasterizerState* mRasterizerState;
        DX11ConstantBuffer<ConstantBufferForward> mConstantBuffer;

        std::vector<DX11MeshPtr> mMeshes;
        std::vector<DX11TexturePtr> mTextures;
    };
}