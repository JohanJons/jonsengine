#include "include/Renderer/DirectX11/DX11ToneMapper.h"

#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/Shaders/Constants.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/Tonemapping.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/AvgLuminance.h"

namespace JonsEngine
{
	

	DX11ToneMapper::DX11ToneMapper(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11FullscreenTrianglePass& fullscreenPass, const EngineSettings::ToneMappingAlghorithm alghorithm, const EngineSettings::AutoExposureRate rate) :
		mContext(context),

		mAvgLuminanceCBuffer(device, context, DX11ConstantBuffer<AvgLuminanceCBuffer>::CONSTANT_BUFFER_SLOT_PIXEL),
		mLuminanceTexture(nullptr),
		mAvgLuminanceBuffers({ nullptr, nullptr }),
		mLuminanceRTVs({ nullptr, nullptr, nullptr }),
		mLuminanceSRVs({ nullptr, nullptr, nullptr }),
		mAvgLuminancePixelShader(nullptr),
		mTonemapPixelShader(nullptr),

		mFullscreenPass(fullscreenPass),
		mAlghorithm(alghorithm),
		mAutoExposureRate(rate)
	{
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
		ZeroMemory(&mAvgLuminanceViewport, sizeof(D3D11_VIEWPORT));
		mAvgLuminanceViewport.TopLeftX = 0;
		mAvgLuminanceViewport.TopLeftY = 0;
		mAvgLuminanceViewport.Width = static_cast<float>(LUM_MAP_WIDTH);
		mAvgLuminanceViewport.Height = static_cast<float>(LUM_MAP_HEIGHT);
		mAvgLuminanceViewport.MinDepth = 0.0f;
		mAvgLuminanceViewport.MaxDepth = 1.0f;
	}


	void DX11ToneMapper::BindAsRenderTarget()
	{
		mContext->OMSetRenderTargets(1, &mLuminanceRTV.p, nullptr);
	}

	void DX11ToneMapper::RenderLuminance(const Milliseconds elapstedFrameTime)
	{
		AverageLumPass(elapstedFrameTime);

		// avg. frame luminance will be in the lowest mip
		mContext->GenerateMips(mLuminanceSRV);
	}

	void DX11ToneMapper::ApplyToneMapping()
	{
		assert(mAlghorithm == EngineSettings::ToneMappingAlghorithm::FilmicU2);

		mContext->PSSetShaderResources(SHADER_TEXTURE_SLOT_EXTRA, 1, &mLuminanceSRV.p);
		mContext->PSSetShader(mTonemapPixelShader, nullptr, 0);
		mFullscreenPass.Render();
	}


	void DX11ToneMapper::AverageLumPass(const Milliseconds elapstedFrameTime)
	{
		D3D11_VIEWPORT prevViewport;
		uint32_t num = 1;
		mContext->RSGetViewports(&num, &prevViewport);

		mContext->RSSetViewports(1, &mAvgLuminanceViewport);
		mContext->ClearRenderTargetView(mLuminanceRTV, GetClearColor());

		mContext->PSSetShader(mAvgLuminancePixelShader, nullptr, 0);
		const float elapsedSeconds = TimeInSeconds(elapstedFrameTime).count();
		mAvgLuminanceCBuffer.SetData({ elapsedSeconds, mAutoExposureRate });

		mFullscreenPass.Render(true);

		mContext->RSSetViewports(1, &prevViewport);
	}
}