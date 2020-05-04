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

		enum class TerrainNormals
		{
			SIMPLE,
			BETTER
		} mTerrainNormals;

		AutoExposureRate mAutoExposureRate;
		bool mBloomEnabled = true;
		bool mSSAOEnabled = true;
		bool mVSync = true;
		float mZNear = 1.0f;
		float mZFar = 1024.0f;

		RenderSettings();
	};

	inline RenderSettings::RenderSettings() :
		mShadowResolution( ShadowResolution::Resolution_2048 ),
		mShadowReadbackLatency( ShadowReadbackLatency::Latency_2 ),
		mShadowFiltering( ShadowFiltering::PCF_2x2 ),
		mAnisotropicFiltering( Anisotropic::X16 ),
		mAntiAliasing( AntiAliasing::Fxaa ),
		mToneMapping( ToneMappingAlghorithm::FilmicU2 ),
		mTerrainPatchMinSize( TerrainPatchMinSize::X16 ),
		mTerrainPatchMaxSize( TerrainPatchMaxSize::X512 ),
		mTerrainMeshDimensions( TerrainMeshDimensions::X16 ),
		mTerrainNormals( TerrainNormals::SIMPLE ),
		mAutoExposureRate(DefaultAutoExposureRate())
	{
	}
}