#pragma once

#include "Core/Types.h"

namespace JonsEngine
{
	struct RenderSettings
	{
		typedef float AutoExposureRate;
		constexpr AutoExposureRate static DefaultAutoExposureRate() { return 1.0f; }


		enum class ShadowResolution
		{
			Resolution_1024,
			Resolution_2048,
			Resolution_4092
		} mShadowResolution;

		enum class ShadowReadbackLatency
		{
			Latency_0,
			Latency_1,
			Latency_2,
			Latency_3
		} mShadowReadbackLatency;

		enum class ShadowFiltering
		{
			PCF_2x2,
			PCF_3x3,
			PCF_5x5,
			PCF_7x7
		} mShadowFiltering;

		enum class Anisotropic
		{
			X1,
			X2,
			X4,
			X8,
			X16
		} mAnisotropicFiltering;

		enum class AntiAliasing
		{
			None,
			Fxaa
		} mAntiAliasing;

		enum class ToneMappingAlghorithm
		{
			None,
			FilmicU2
		} mToneMapping;

        enum class TerrainPatchSize
        {
            X16,
            X32
        } mTerrainPatchSize;

		// units in screenspace for maximum tessellation
		enum class TerrainPrimitiveLength
		{
			X32,
			X48,
			X64,
			X80,
			X96,
			X112
		} mTerrainPrimitiveLength;

		// max tessellation given by screenspace primtive length
		enum class TerrainTessellationMax
		{
			X24,
			X32,
			X40,
			X48,
			X56,
			X64
		} mTerrainTessellationMax;

		// tessellation coplanarity scaling
		enum class TerrainCoplanarityScale
		{
			X05,
			X1,
			X2,
			X3
		} mTerrainCoplanarityScale;

		AutoExposureRate mAutoExposureRate;
		bool mBloomEnabled;
		bool mSSAOEnabled;
        bool mVSync;


		RenderSettings();
	};

	inline RenderSettings::RenderSettings() :
		mShadowResolution( ShadowResolution::Resolution_2048 ),
		mShadowReadbackLatency( ShadowReadbackLatency::Latency_2 ),
		mShadowFiltering( ShadowFiltering::PCF_2x2 ),
		mAnisotropicFiltering( Anisotropic::X16 ),
		mAntiAliasing( AntiAliasing::Fxaa ),
		mToneMapping( ToneMappingAlghorithm::FilmicU2 ),
		mTerrainPatchSize( TerrainPatchSize::X32 ),
		mTerrainPrimitiveLength( TerrainPrimitiveLength::X64 ),
		mTerrainTessellationMax( TerrainTessellationMax::X48 ),
		mTerrainCoplanarityScale( TerrainCoplanarityScale::X1 ),
		mAutoExposureRate(DefaultAutoExposureRate()),
		mBloomEnabled(true),
		mSSAOEnabled(true),
        mVSync(true)
	{
	}
}