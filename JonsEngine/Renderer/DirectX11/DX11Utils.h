#pragma once

#include "Shaders/Constants.hlsl"
#include "Renderer/RenderSettings.h"
#include "Core/Logging/Logger.h"
#include "Core/EngineSettings.h"
#include "Core/Types.h"
#include "Core/Platform.h"

#include "boost/current_function.hpp"
#include <atlbase.h>
#include <sstream>
#include <exception>
#include <array>

namespace JonsEngine
{
    typedef std::array<float, 4> DX11Color;
	constexpr const DX11Color gClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	constexpr const float* GetClearColor() { return &gClearColor.front(); };


    enum SHADER_TEXTURE_SLOT
    {
        SHADER_TEXTURE_SLOT_DIFFUSE = TEXTURE_SLOT_DIFFUSE,
        SHADER_TEXTURE_SLOT_NORMAL = TEXTURE_SLOT_NORMAL,
        SHADER_TEXTURE_SLOT_DEPTH = TEXTURE_SLOT_DEPTH,
		SHADER_TEXTURE_SLOT_PERLIN = TEXTURE_SLOT_PERLIN,
        SHADER_TEXTURE_SLOT_EXTRA = TEXTURE_SLOT_EXTRA,
        SHADER_TEXTURE_SLOT_EXTRA_2 = TEXTURE_SLOT_EXTRA_2,
        NUM_SHADER_TEXTURE_SLOTS
    };

    uint32_t RenderSettingsToVal( RenderSettings::Anisotropic anisotropicEnum );
    uint32_t RenderSettingsToVal( RenderSettings::ShadowReadbackLatency shadowReadbackLatencyEnum );
    uint32_t RenderSettingsToVal( RenderSettings::ShadowResolution shadowResolutionEnum );
	uint32_t RenderSettingsToVal( RenderSettings::TerrainPatchMinSize patchSizeEnum );
	uint32_t RenderSettingsToVal( RenderSettings::TerrainPatchMaxSize patchSizeEnum );
	uint32_t RenderSettingsToVal( RenderSettings::TerrainMeshDimensions meshDimensionsEnum );

    #ifdef _DEBUG
    #define DXCALL(function)                                                                                                                            \
    {                                                                                                                                                   \
        HRESULT result = function;                                                                                                                      \
        if (FAILED(result))                                                                                                                             \
        {                                                                                                                                               \
            std::stringstream errorSS;                                                                                                                  \
            errorSS << "D3D11 Error: (" << __FILE__ << ":" << __LINE__ << ") in " << BOOST_CURRENT_FUNCTION << ": code " << result;                     \
            Logger::GetRendererLogger().Log(LogManager::LogLevel::Error, errorSS.str());                                                                \
            throw new std::runtime_error(errorSS.str());                                                                                                \
        }                                                                                                                                               \
    }
    #else
    #define DXCALL(function) function;
    #endif

    // COM smart pointers
    typedef CComPtr<ID3D11Buffer> ID3D11BufferPtr;
    typedef CComPtr<ID3D11VertexShader> ID3D11VertexShaderPtr;
	typedef CComPtr<ID3D11DomainShader> ID3D11DomainShaderPtr;
	typedef CComPtr<ID3D11HullShader> ID3D11HullShaderPtr;
    typedef CComPtr<ID3D11PixelShader> ID3D11PixelShaderPtr;
    typedef CComPtr<ID3D11ComputeShader> ID3D11ComputeShaderPtr;
    typedef CComPtr<ID3D11RasterizerState> ID3D11RasterizerStatePtr;
    typedef CComPtr<ID3D11BlendState> ID3D11BlendStatePtr;
    typedef CComPtr<ID3D11SamplerState> ID3D11SamplerStatePtr;
    typedef CComPtr<ID3D11RenderTargetView> ID3D11RenderTargetViewPtr;
    typedef CComPtr<ID3D11DepthStencilState> ID3D11DepthStencilStatePtr;
    typedef CComPtr<ID3D11DepthStencilView> ID3D11DepthStencilViewPtr;
    typedef CComPtr<IDXGISwapChain> IDXGISwapChainPtr;
    typedef CComPtr<ID3D11Device> ID3D11DevicePtr;
    typedef CComPtr<ID3D11DeviceContext> ID3D11DeviceContextPtr;
	typedef CComPtr<ID3D11Texture1D> ID3D11Texture1DPtr;
    typedef CComPtr<ID3D11Texture2D> ID3D11Texture2DPtr;
    typedef CComPtr<ID3D11ShaderResourceView> ID3D11ShaderResourceViewPtr;
    typedef CComPtr<ID3D11UnorderedAccessView> ID3D11UnorderedAccessViewPtr;
    typedef CComPtr<ID3D11InputLayout> ID3D11InputLayoutPtr;

    static ID3D11ShaderResourceViewPtr gNullSRV = nullptr;
	static ID3D11UnorderedAccessViewPtr gNullUAV = nullptr;
}
