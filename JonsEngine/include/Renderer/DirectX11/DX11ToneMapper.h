#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Core/Utils/Time.h"

#include "include/Core/EngineSettings.h"

#include <array>

namespace JonsEngine
{
	class DX11FullscreenTrianglePass;

	class DX11ToneMapper
	{
	public:
		DX11ToneMapper(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11FullscreenTrianglePass& fullscreenPass, const EngineSettings::ToneMappingAlghorithm alghorithm, const EngineSettings::AutoExposureRate rate);

		void BindAsRenderTarget();
		void RenderLuminance(const Milliseconds elapstedFrameTime);
		void ApplyToneMapping();


	private:
		struct AvgLuminanceCBuffer
		{
			float mTimeDelta;
			float mAdaptationRate;
			float __padding[2];
		};

		void AverageLumPass(const Milliseconds elapstedFrameTime);
		void TonemappingPass();


		ID3D11DeviceContextPtr mContext;

		D3D11_VIEWPORT mAvgLuminanceViewport;
		DX11ConstantBuffer<AvgLuminanceCBuffer> mAvgLuminanceCBuffer;
		ID3D11SamplerStatePtr mLinearSampler;
		ID3D11Texture2DPtr mLuminanceTexture;
		std::array<ID3D11BufferPtr, 2> mAvgLuminanceBuffers;
		std::array<ID3D11RenderTargetViewPtr, 3> mLuminanceRTVs;
		std::array<ID3D11ShaderResourceViewPtr, 3> mLuminanceSRVs;
		ID3D11PixelShaderPtr mAvgLuminancePixelShader;
		ID3D11PixelShaderPtr mTonemapPixelShader;

		DX11FullscreenTrianglePass& mFullscreenPass;

		EngineSettings::ToneMappingAlghorithm mAlghorithm;
		EngineSettings::AutoExposureRate mAutoExposureRate;
	};
}