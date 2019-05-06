#include "Renderer/DirectX11/DX11ToneMapper.h"

#include "Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "Shaders/Constants.hlsl"
#include "Compiled/Tonemapping.h"
#include "Compiled/AvgLuminance.h"

namespace JonsEngine
{
	

	DX11ToneMapper::DX11ToneMapper(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11FullscreenTrianglePass& fullscreenPass) :
		mContext(context),

		mAvgLuminanceCBuffer(device, context, DX11ConstantBuffer<AvgLuminanceCBuffer>::CONSTANT_BUFFER_SLOT_PIXEL),
		mTonemappingCBuffer(device, context, DX11ConstantBuffer<TonemappingCBuffer>::CONSTANT_BUFFER_SLOT_PIXEL),

		mLuminanceTextures({ nullptr, nullptr }),
		mLuminanceRTVs({ nullptr, nullptr }),
		mLuminanceSRVs({ nullptr, nullptr }),
		mCurrentLumTexture(0),

		mAvgLuminancePixelShader(nullptr),
		mTonemapPixelShader(nullptr),

		mFullscreenPass(fullscreenPass)
	{
		D3D11_TEXTURE2D_DESC lumDesc;
		ZeroMemory(&lumDesc, sizeof(D3D11_TEXTURE2D_DESC));
		lumDesc.Width = LUM_MAP_WIDTH;
		lumDesc.Height = LUM_MAP_HEIGHT;
		lumDesc.ArraySize = 1;
		lumDesc.Format = DXGI_FORMAT_R16_FLOAT;
		lumDesc.SampleDesc.Count = 1;
		lumDesc.Usage = D3D11_USAGE_DEFAULT;
		lumDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		lumDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		
		const std::array<float, 4> avgLuminanceClearColor = { 0.0001f, 0.0f, 0.0f, 0.0f };
		for (uint32_t lumTextureIndex = 0; lumTextureIndex < NumLuminanceTextures; ++lumTextureIndex)
		{
			auto& lumTexture = mLuminanceTextures.at(lumTextureIndex);
			auto& lumRTV = mLuminanceRTVs.at(lumTextureIndex);
			auto& lumSRV = mLuminanceSRVs.at(lumTextureIndex);

			DXCALL(device->CreateTexture2D(&lumDesc, nullptr, &lumTexture));
			DXCALL(device->CreateRenderTargetView(lumTexture, nullptr, &lumRTV));
			DXCALL(device->CreateShaderResourceView(lumTexture, nullptr, &lumSRV));

			mContext->ClearRenderTargetView(lumRTV, &avgLuminanceClearColor.front());
		}

		DXCALL(device->CreatePixelShader(gAvgLuminance, sizeof(gAvgLuminance), nullptr, &mAvgLuminancePixelShader));
		DXCALL(device->CreatePixelShader(gTonemapping, sizeof(gTonemapping), nullptr, &mTonemapPixelShader));

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

	void DX11ToneMapper::RenderLuminance(const Milliseconds elapstedFrameTime, const RenderSettings::AutoExposureRate exposureRate)
	{
		D3D11_VIEWPORT prevViewport;
		uint32_t num = 1;
		mContext->RSGetViewports(&num, &prevViewport);

		mContext->RSSetViewports(1, &mAvgLuminanceViewport);
		mContext->ClearRenderTargetView(mLuminanceRTVs.at(mCurrentLumTexture), GetClearColor());

		mContext->PSSetShader(mAvgLuminancePixelShader, nullptr, 0);
		mContext->PSSetShaderResources(SHADER_TEXTURE_SLOT_EXTRA, 1, &mLuminanceSRVs.at(!mCurrentLumTexture).p);
		const float elapsedSeconds = ConvertTimeUnitToFloat(TimeInSeconds(elapstedFrameTime));
		mAvgLuminanceCBuffer.SetData({ elapsedSeconds, exposureRate });
		mAvgLuminanceCBuffer.Bind();

		mFullscreenPass.RenderWithTexcoords();

		mContext->RSSetViewports(1, &prevViewport);

		// avg. frame luminance will be in the lowest mip
		mContext->GenerateMips(mLuminanceSRVs.at(mCurrentLumTexture));
	}

	void DX11ToneMapper::ApplyToneMapping(const RenderSettings::ToneMappingAlghorithm alghorithm)
	{
		static_assert(NumLuminanceTextures == 2, "Code assumes only 2 luminance textures");
		static_assert(LUM_MAP_WIDTH == LUM_MAP_HEIGHT, "Code assumes 1:1 luminance texture aspect ratio");
		assert(alghorithm == RenderSettings::ToneMappingAlghorithm::FilmicU2);

		mContext->PSSetShaderResources(SHADER_TEXTURE_SLOT_EXTRA, 1, &mLuminanceSRVs.at(mCurrentLumTexture).p);
		mContext->PSSetShader(mTonemapPixelShader, nullptr, 0);
		const uint32_t mipMapLevel = static_cast<uint32_t>(std::log2(LUM_MAP_WIDTH));
		mTonemappingCBuffer.SetData({ mipMapLevel });
		mTonemappingCBuffer.Bind();

		mFullscreenPass.RenderWithTexcoords();

		mCurrentLumTexture = !mCurrentLumTexture;
	}
}