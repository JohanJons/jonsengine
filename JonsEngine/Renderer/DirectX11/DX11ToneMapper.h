#pragma once

#include "DX11Utils.h"
#include "DX11ConstantBuffer.hpp"
#include "Time.h"

#include "EngineSettings.h"

#include <array>

namespace JonsEngine
{
	class DX11FullscreenTrianglePass;

	class DX11ToneMapper
	{
	public:
		DX11ToneMapper(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11FullscreenTrianglePass& fullscreenPass);

		void BindAsRenderTarget();
		void RenderLuminance(const Milliseconds elapstedFrameTime, const RenderSettings::AutoExposureRate exposureRate);
		void ApplyToneMapping(const RenderSettings::ToneMappingAlghorithm alghorithm);


	private:
		struct AvgLuminanceCBuffer
		{
			float mTimeDelta;
			float mAdaptationRate;
			float __padding[2];
		};

		struct TonemappingCBuffer
		{
			uint32_t mMipMapLevel;
			float __padding[3];
		};

		static constexpr uint32_t NumLuminanceTextures = 2;


		ID3D11DeviceContextPtr mContext;

		D3D11_VIEWPORT mAvgLuminanceViewport;
		DX11ConstantBuffer<AvgLuminanceCBuffer> mAvgLuminanceCBuffer;
		DX11ConstantBuffer<TonemappingCBuffer> mTonemappingCBuffer;

		std::array<ID3D11Texture2DPtr, NumLuminanceTextures> mLuminanceTextures;
		std::array<ID3D11RenderTargetViewPtr, NumLuminanceTextures> mLuminanceRTVs;
		std::array<ID3D11ShaderResourceViewPtr, NumLuminanceTextures> mLuminanceSRVs;
		uint32_t mCurrentLumTexture;
		
		ID3D11PixelShaderPtr mAvgLuminancePixelShader;
		ID3D11PixelShaderPtr mTonemapPixelShader;

		DX11FullscreenTrianglePass& mFullscreenPass;
	};
}