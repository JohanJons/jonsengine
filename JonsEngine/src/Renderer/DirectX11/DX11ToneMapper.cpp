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
		mTonemappingCBuffer(device, context, DX11ConstantBuffer<TonemappingCBuffer>::CONSTANT_BUFFER_SLOT_PIXEL),

		mLuminanceTextures({ nullptr, nullptr }),
		mLuminanceRTVs({ nullptr, nullptr }),
		mLuminanceSRVs({ nullptr, nullptr }),
		mCurrentLumTexture(0),

		mAvgLuminancePixelShader(nullptr),
		mTonemapPixelShader(nullptr),

		mFullscreenPass(fullscreenPass),
		mAlghorithm(alghorithm),
		mAutoExposureRate(rate)
	{
		D3D11_TEXTURE2D_DESC lumDesc;
		ZeroMemory(&lumDesc, sizeof(D3D11_TEXTURE2D_DESC));
		lumDesc.Width = LUM_MAP_WIDTH;
		lumDesc.Height = LUM_MAP_HEIGHT;
		lumDesc.MipLevels = 1;
		lumDesc.ArraySize = 1;
		lumDesc.Format = DXGI_FORMAT_R16_FLOAT;
		lumDesc.SampleDesc.Count = 1;
		lumDesc.Usage = D3D11_USAGE_DEFAULT;
		lumDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		lumDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		
		for (uint32_t lumTextureIndex = 0; lumTextureIndex < NumLuminanceTextures; ++lumTextureIndex)
		{
			auto& lumTexture = mLuminanceTextures.at(lumTextureIndex);
			auto& lumRTV = mLuminanceRTVs.at(lumTextureIndex);
			auto& lumSRV = mLuminanceSRVs.at(lumTextureIndex);

			DXCALL(device->CreateTexture2D(&lumDesc, nullptr, &lumTexture));
			DXCALL(device->CreateRenderTargetView(lumTexture, nullptr, &lumRTV));
			DXCALL(device->CreateShaderResourceView(lumTexture, nullptr, &lumSRV));

			mContext->ClearRenderTargetView(lumRTV, GetClearColor());
		}

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
		mContext->OMSetRenderTargets(1, &mLuminanceRTVs.at(mCurrentLumTexture).p, nullptr);
	}

	void DX11ToneMapper::RenderLuminance(const Milliseconds elapstedFrameTime)
	{
		AverageLumPass(elapstedFrameTime);

		// avg. frame luminance will be in the lowest mip
		mContext->GenerateMips(mLuminanceSRVs.at(mCurrentLumTexture));
	}

	void DX11ToneMapper::ApplyToneMapping()
	{
		static_assert(NumLuminanceTextures == 2, "Code assumes only 2 luminance textures");
		static_assert(LUM_MAP_WIDTH == LUM_MAP_HEIGHT, "Code assumes 1:1 lum texture aspect ratio");
		assert(mAlghorithm == EngineSettings::ToneMappingAlghorithm::FilmicU2);

		mContext->PSSetShaderResources(SHADER_TEXTURE_SLOT_EXTRA, 1, &mLuminanceSRVs.at(mCurrentLumTexture).p);
		mContext->PSSetShader(mTonemapPixelShader, nullptr, 0);
		const uint32_t mipMapLevel = std::log(LUM_MAP_WIDTH);
		mTonemappingCBuffer.SetData({ mipMapLevel });

		mFullscreenPass.Render();

		mCurrentLumTexture = !mCurrentLumTexture;
	}


	void DX11ToneMapper::AverageLumPass(const Milliseconds elapstedFrameTime)
	{
		D3D11_VIEWPORT prevViewport;
		uint32_t num = 1;
		mContext->RSGetViewports(&num, &prevViewport);

		mContext->RSSetViewports(1, &mAvgLuminanceViewport);
		mContext->ClearRenderTargetView(mLuminanceRTVs.at(mCurrentLumTexture), GetClearColor());

		mContext->PSSetShader(mAvgLuminancePixelShader, nullptr, 0);
		// note: first frame prev lum texture will be black. Problem?
		mContext->PSSetShaderResources(SHADER_TEXTURE_SLOT_EXTRA, 1, &mLuminanceSRVs.at(!mCurrentLumTexture).p);
		const float elapsedSeconds = ConvertTimeUnitToFloat(TimeInSeconds(elapstedFrameTime));
		mAvgLuminanceCBuffer.SetData({ elapsedSeconds, mAutoExposureRate });

		mFullscreenPass.RenderWithTexcoords();

		mContext->RSSetViewports(1, &prevViewport);
	}
}