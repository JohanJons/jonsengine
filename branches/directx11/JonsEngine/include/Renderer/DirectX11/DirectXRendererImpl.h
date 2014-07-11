#pragma once

#include "include/Renderer/RenderCommands.h"
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

    class DirectXRendererImpl
    {
    public:
        DirectXRendererImpl(const EngineSettings& settings, Logger& logger);
        ~DirectXRendererImpl();

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
        static LRESULT CALLBACK DirectXRendererImpl::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
        Logger& mLogger;

        HWND mWindowHandle;
        IDXGISwapChain* mSwapchain;
        ID3D11RenderTargetView* mBackbuffer;
        ID3D11Device* mDevice;
        ID3D11DeviceContext* mContext;

        ID3D11VertexShader* mForwardVertexShader;
        ID3D11PixelShader* mForwardPixelShader;
		ID3D11Buffer* mVertexBuffer;
		ID3D11InputLayout* mInputLayout;
    };
}