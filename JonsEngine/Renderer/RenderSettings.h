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

		enum class TerrainPatchMinSize
		{
			X2,
			X4,
			X8,
			X16,
			X32
		} mTerrainPatchMinSize;

		enum class TerrainPatchMaxSize
		{
			X16,
			X32,
			X64,
			X128,
			X256,
			X512,
			X1024
		} mTerrainPatchMaxSize;

		enum class TerrainMeshDimensions
		{
			X4,
			X8,
			X16,
			X32,
			X64
		} mTerrainMeshDimensions;

		enum class TerrainCoplanaritySize
		{
			X8,
			X16,
			X32
		} mTerrainCoplanaritySize;

		// units in screenspace for maximum tessellation
		enum class TerrainPrimitiveLength
		{
			X64,
			X96,
			X128,
			X160,
			X192,
			X224
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
			X1_5,
			X2
		} mTerrainCoplanarityScale;

		enum class TerrainCoplanarityTessellationRatio
		{
			X03,
			X04,
			X05,
			X06,
			X065,
			X07,
			X075,
			X08
		} mTerrainCoplanarityTessellationRatio;

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
		mTerrainPatchMinSize( TerrainPatchMinSize::X2 ),
		mTerrainPatchMaxSize( TerrainPatchMaxSize::X128 ),
		mTerrainMeshDimensions( TerrainMeshDimensions::X16 ),
		mTerrainCoplanaritySize( TerrainCoplanaritySize::X16 ),
		mTerrainPrimitiveLength( TerrainPrimitiveLength::X160 ),
		mTerrainTessellationMax( TerrainTessellationMax::X64 ),
		mTerrainCoplanarityScale( TerrainCoplanarityScale::X1_5 ),
		mTerrainCoplanarityTessellationRatio( TerrainCoplanarityTessellationRatio::X06 ),
		mAutoExposureRate(DefaultAutoExposureRate()),
		mBloomEnabled(true),
		mSSAOEnabled(true),
        mVSync(true)
	{
	}
}