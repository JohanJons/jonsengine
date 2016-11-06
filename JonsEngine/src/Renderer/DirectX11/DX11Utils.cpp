#include "include/Renderer/DirectX11/DX11Utils.h"

#include "include/Core/Utils/Error.h"

namespace JonsEngine
{
    uint32_t EngineSettingsToVal(const EngineSettings::Anisotropic anisotropicEnum)
    {
        switch (anisotropicEnum)
        {
            default:
                JONS_ERROR(Logger::GetRendererLogger(), "DX11Utils::EngineSettingsToVal(EngineSettings::Anisotropic): Invalid enum");
            case EngineSettings::Anisotropic::X1:
                return 1;
            case EngineSettings::Anisotropic::X2:
                return 2;
            case EngineSettings::Anisotropic::X4:
                return 4;
            case EngineSettings::Anisotropic::X8:
                return 8;
            case EngineSettings::Anisotropic::X16:
                return 16;
        }
    }

    uint32_t EngineSettingsToVal(const EngineSettings::ShadowReadbackLatency shadowReadbackLatencyEnum)
    {
        switch (shadowReadbackLatencyEnum)
        {
            default:
                JONS_ERROR(Logger::GetRendererLogger(), "DX11Utils::EngineSettingsToVal(EngineSettings::ShadowReadbackLatency): Invalid enum");
            case EngineSettings::ShadowReadbackLatency::Latency_0:
                return 0;
            case EngineSettings::ShadowReadbackLatency::Latency_1:
                return 1;
            case EngineSettings::ShadowReadbackLatency::Latency_2:
                return 2;
            case EngineSettings::ShadowReadbackLatency::Latency_3:
                return 3;
        }
    }

    uint32_t EngineSettingsToVal(const EngineSettings::ShadowResolution shadowResolutionEnum)
    {
        switch (shadowResolutionEnum)
        {
            default:
                JONS_ERROR(Logger::GetRendererLogger(), "DX11Utils::EngineSettingsToVal(EngineSettings::ShadowResolution): Invalid enum");
            case EngineSettings::ShadowResolution::Resolution_1024:
                return 1024;
            case EngineSettings::ShadowResolution::Resolution_2048:
                return 2048;
            case EngineSettings::ShadowResolution::Resolution_4092:
                return 4092;
        }
    }
}
