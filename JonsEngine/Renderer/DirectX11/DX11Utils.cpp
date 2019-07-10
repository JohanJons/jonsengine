#include "Renderer/DirectX11/DX11Utils.h"

#include "Core/Utils/Error.h"

namespace JonsEngine
{
    uint32_t RenderSettingsToVal( RenderSettings::Anisotropic anisotropicEnum )
    {
        switch (anisotropicEnum)
        {
            default:
                JONS_ERROR(Logger::GetRendererLogger(), "DX11Utils::RenderSettingsToVal(RenderSettings::Anisotropic): Invalid enum");
            case RenderSettings::Anisotropic::X1:
                return 1;
            case RenderSettings::Anisotropic::X2:
                return 2;
            case RenderSettings::Anisotropic::X4:
                return 4;
            case RenderSettings::Anisotropic::X8:
                return 8;
            case RenderSettings::Anisotropic::X16:
                return 16;
        }
    }

    uint32_t RenderSettingsToVal( RenderSettings::ShadowReadbackLatency shadowReadbackLatencyEnum )
    {
        switch (shadowReadbackLatencyEnum)
        {
            default:
                JONS_ERROR(Logger::GetRendererLogger(), "DX11Utils::RenderSettingsToVal(RenderSettings::ShadowReadbackLatency): Invalid enum");
            case RenderSettings::ShadowReadbackLatency::Latency_0:
                return 0;
            case RenderSettings::ShadowReadbackLatency::Latency_1:
                return 1;
            case RenderSettings::ShadowReadbackLatency::Latency_2:
                return 2;
            case RenderSettings::ShadowReadbackLatency::Latency_3:
                return 3;
        }
    }

    uint32_t RenderSettingsToVal( RenderSettings::ShadowResolution shadowResolutionEnum )
    {
        switch (shadowResolutionEnum)
        {
            default:
                JONS_ERROR(Logger::GetRendererLogger(), "DX11Utils::RenderSettingsToVal(RenderSettings::ShadowResolution): Invalid enum");
            case RenderSettings::ShadowResolution::Resolution_1024:
                return 1024;
            case RenderSettings::ShadowResolution::Resolution_2048:
                return 2048;
            case RenderSettings::ShadowResolution::Resolution_4092:
                return 4092;
        }
    }

    uint32_t RenderSettingsToVal( RenderSettings::TerrainPatchSize patchSizeEnum )
    {
        switch ( patchSizeEnum )
        {
            case RenderSettings::TerrainPatchSize::X16: return 16;
            case RenderSettings::TerrainPatchSize::X32: return 32;
            default:
			{
				JONS_ERROR( Logger::GetRendererLogger(), "DX11Utils::RenderSettingsToVal(RenderSettings::TerrainPatchSize): Invalid enum" );
				return 0;
			}

        }
    }

	uint32_t RenderSettingsToVal( RenderSettings::TerrainPrimitiveLength primitiveLength )
	{
		switch ( primitiveLength )
		{
			case RenderSettings::TerrainPrimitiveLength::X64: return 64;
			case RenderSettings::TerrainPrimitiveLength::X96: return 96;
			case RenderSettings::TerrainPrimitiveLength::X128: return 128;
			case RenderSettings::TerrainPrimitiveLength::X160: return 160;
			case RenderSettings::TerrainPrimitiveLength::X192: return 192;
			case RenderSettings::TerrainPrimitiveLength::X224: return 224;
			default:
			{
				JONS_ERROR( Logger::GetRendererLogger(), "DX11Utils::RenderSettingsToVal(RenderSettings::TerrainPrimitiveLength): Invalid enum" );
				return 0;
			}
		}
	}

	uint32_t RenderSettingsToVal( RenderSettings::TerrainTessellationMax TerrainTessellationMax )
	{
		switch ( TerrainTessellationMax )
		{
			case RenderSettings::TerrainTessellationMax::X24: return 24;
			case RenderSettings::TerrainTessellationMax::X32: return 32;
			case RenderSettings::TerrainTessellationMax::X40: return 40;
			case RenderSettings::TerrainTessellationMax::X48: return 48;
			case RenderSettings::TerrainTessellationMax::X56: return 56;
			case RenderSettings::TerrainTessellationMax::X64: return 64;
			default:
			{
				JONS_ERROR( Logger::GetRendererLogger(), "DX11Utils::RenderSettingsToVal(RenderSettings::TerrainTessellationMax): Invalid enum" );
				return 0;
			}
		}
	}

	float RenderSettingsToVal( RenderSettings::TerrainCoplanarityScale coplanarityScale )
	{
		switch ( coplanarityScale )
		{
			case RenderSettings::TerrainCoplanarityScale::X05: return 0.5f;
			case RenderSettings::TerrainCoplanarityScale::X1: return 1.0f;
			case RenderSettings::TerrainCoplanarityScale::X1_5: return 1.5f;
			case RenderSettings::TerrainCoplanarityScale::X2: return 2.0f;
			default:
			{
				JONS_ERROR( Logger::GetRendererLogger(), "DX11Utils::RenderSettingsToVal(RenderSettings::TerrainCoplanarityScale): Invalid enum" );
				return 0.0f;
			}
		}
	}

	float RenderSettingsToVal( RenderSettings::TerrainCoplanarityTessellationRatio coplanarityTessRatio )
	{
		switch ( coplanarityTessRatio )
		{
			case RenderSettings::TerrainCoplanarityTessellationRatio::X03: return 0.3f;
			case RenderSettings::TerrainCoplanarityTessellationRatio::X04: return 0.4f;
			case RenderSettings::TerrainCoplanarityTessellationRatio::X05: return 0.5f;
			case RenderSettings::TerrainCoplanarityTessellationRatio::X06: return 0.6f;
			case RenderSettings::TerrainCoplanarityTessellationRatio::X065: return 0.65f;
			case RenderSettings::TerrainCoplanarityTessellationRatio::X07: return 0.7f;
			case RenderSettings::TerrainCoplanarityTessellationRatio::X075: return 0.75f;
			case RenderSettings::TerrainCoplanarityTessellationRatio::X08: return 0.8f;
			default:
			{
				JONS_ERROR( Logger::GetRendererLogger(), "DX11Utils::RenderSettingsToVal(RenderSettings::TerrainCoplanarityTessellationRatio): Invalid enum" );
				return 0.0f;
			}
		}
	}
}
