#include "include/Renderer/DirectX11/DX11ToneMapper.h"

#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/Shaders/Constants.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/Tonemapping.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/AvgLuminance.h"

namespace JonsEngine
{
	DX11ToneMapper::DX11ToneMapper(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11FullscreenTrianglePass& fullscreenPass, const EngineSettings::ToneMappingAlghorithm alghorithm, const EngineSettings::ToneMappingAdaptationRate rate) :
		mContext(context),

		mAvgLuminanceCBuffer(device, context, DX11ConstantBuffer<AvgLuminanceCBuffer>::CONSTANT_BUFFER_SLOT_PIXEL),
		mLinearSampler(nullptr),
		mLuminanceTexture(nullptr),
		mLuminanceRTV(nullptr),
		mLuminanceSRV(nullptr),
		mAvgLuminancePixelShader(nullptr),
		mTonemapPixelShader(nullptr),

		mFullscreenPass(fullscreenPass),

		mAlghorithm(alghorithm),
		mAdaptationRate(rate)
	{
		// Create sampler states
		D3D11_SAMPLER_DESC sampDesc;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.BorderColor[0] = 0;
		sampDesc.BorderColor[1] = 0;
		sampDesc.BorderColor[2] = 0;
		sampDesc.BorderColor[3] = 0;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.MaxAnisotropy = 1;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		sampDesc.MinLOD = 0;
		sampDesc.MipLODBias = 0;
		DXCALL(device->CreateSamplerState(&sampDesc, &mLinearSampler));

		D3D11_TEXTURE2D_DESC lumTextureDesc;
		ZeroMemory(&lumTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
		lumTextureDesc.Width = LUM_MAP_WIDTH;
		lumTextureDesc.Height = LUM_MAP_HEIGHT;
		lumTextureDesc.MipLevels = 1;
		lumTextureDesc.ArraySize = 1;
		lumTextureDesc.Format = DXGI_FORMAT_R16_FLOAT;
		lumTextureDesc.SampleDesc.Count = 1;
		lumTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		lumTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		DXCALL(device->CreateTexture2D(&lumTextureDesc, nullptr, &mLuminanceTexture));
		DXCALL(device->CreateRenderTargetView(mLuminanceTexture, nullptr, &mLuminanceRTV));
		DXCALL(device->CreateShaderResourceView(mLuminanceTexture, nullptr, &mLuminanceSRV));

		DXCALL(device->CreatePixelShader(gAvgLuminanceShader, sizeof(gAvgLuminanceShader), nullptr, &mAvgLuminancePixelShader));
		DXCALL(device->CreatePixelShader(gTonemappingShader, sizeof(gTonemappingShader), nullptr, &mTonemapPixelShader));

		// viewport used during shadow pass
		ZeroMemory(&mAvgLumViewport, sizeof(D3D11_VIEWPORT));
		mAvgLumViewport.TopLeftX = 0;
		mAvgLumViewport.TopLeftY = 0;
		mAvgLumViewport.Width = static_cast<float>(LUM_MAP_WIDTH);
		mAvgLumViewport.Height = static_cast<float>(LUM_MAP_HEIGHT);
		mAvgLumViewport.MinDepth = 0.0f;
		mAvgLumViewport.MaxDepth = 1.0f;
	}


	void DX11ToneMapper::BindAsRenderTarget()
	{
		mContext->OMSetRenderTargets(1, &mLuminanceRTV.p, nullptr);
	}

	void DX11ToneMapper::RenderLuminance()
	{
		D3D11_VIEWPORT prevViewport;
		uint32_t num = 1;
		mContext->RSGetViewports(&num, &prevViewport);

		mContext->RSSetViewports(1, &mAvgLumViewport);
		mContext->ClearRenderTargetView(mLuminanceRTV, GetClearColor());

		ID3D11SamplerState* samplerStates[1] = { mLinearSampler };
		mContext->PSSetSamplers(3, 1, samplerStates);

		mContext->PSSetShader(mAvgLuminancePixelShader, nullptr, 0);
		mAvgLuminanceCBuffer.SetData(AvgLuminanceCBuffer());

		mFullscreenPass.Render(true);

		mContext->RSSetViewports(1, &prevViewport);
		//mContext->GenerateMips(mLuminanceSRV);
	}

	void DX11ToneMapper::ApplyToneMapping()
	{
		mContext->PSSetShaderResources(SHADER_TEXTURE_SLOT_EXTRA, 1, &mLuminanceSRV.p);
		mContext->PSSetShader(mTonemapPixelShader, nullptr, 0);
		mFullscreenPass.Render();
	}
}