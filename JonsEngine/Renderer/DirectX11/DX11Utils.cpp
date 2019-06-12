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

    uint32_t RenderSettingsToVal( const RenderSettings::TerrainPatchSize patchSizeEnum )
    {
        switch ( patchSizeEnum )
        {
            case RenderSettings::TerrainPatchSize::X16: return 16;
            case RenderSettings::TerrainPatchSize::X32: return 32;
            default: JONS_ERROR(Logger::GetRendererLogger(), "DX11Utils::RenderSettingsToVal(RenderSettings::TerrainPatchSize): Invalid enum");
        }
    }
}
