#pragma once

#include "include/Core/Types.h"

#include <string>

namespace JonsEngine
{
    /*
     * EngineSettings is used when bootstraping the engine. 
     * Parameters can be changed to alter the engine properties
     */

    /* EngineSettings definition */
    struct EngineSettings
    {
		typedef float AutoExposureRate;
		constexpr AutoExposureRate static DefaultAutoExposureRate() { return 1.0f; }


        // render settings
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
        
        AutoExposureRate mAutoExposureRate;
        bool mBloomEnabled;
        bool mSSAOEnabled;

        // window settings
        std::string mWindowTitle;
        uint16_t mWindowWidth;
        uint16_t mWindowHeight;
        bool mFullscreen;
        uint16_t mFrameLimit;

        // scene settings
        enum class CullingStrategy
        {
            STANDARD,
            AGGRESSIVE
        } mSceneCullingStrategy;


        EngineSettings();
    };


    /* EngineSettings inlines */
	inline EngineSettings::EngineSettings() :
		// render settings
		mShadowResolution(ShadowResolution::Resolution_2048),
		mShadowReadbackLatency(ShadowReadbackLatency::Latency_2),
		mShadowFiltering(ShadowFiltering::PCF_2x2),
		mAnisotropicFiltering(Anisotropic::X16),
		mAntiAliasing(AntiAliasing::Fxaa),
		mToneMapping(ToneMappingAlghorithm::FilmicU2),
		mAutoExposureRate(DefaultAutoExposureRate()),
        mBloomEnabled(true),
        mSSAOEnabled(true),

        // window settings
        mWindowTitle("JonsEngine Game"),
        mWindowWidth(1920),
        mWindowHeight(1080),
        mFullscreen(false),
        mFrameLimit(0),

        // scene settings
        mSceneCullingStrategy(CullingStrategy::STANDARD)
    {
    }
}