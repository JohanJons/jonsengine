#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/EngineSettings.h"
#include "include/Core/Types.h"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11Sampler
    {
    public:
        enum SHADER_SAMPLER_SLOT
        {
            SHADER_SAMPLER_SLOT_MODEL = SAMPLER_SLOT_MODEL,
            SHADER_SAMPLER_SLOT_DEPTH = SAMPLER_SLOT_DEPTH
        };
    
        DX11Sampler(ID3D11DevicePtr device, const EngineSettings::Anisotropic maxAnisotropy, const D3D11_FILTER filter, const SHADER_SAMPLER_SLOT samplerSlot);
        ~DX11Sampler();
        
        void BindSampler(ID3D11DeviceContextPtr context);
        void GetMaxAnisotropicFiltering() const;
        
    
    private:
        ID3D11SamplerStatePtr mTextureSampler;
        const EngineSettings::Anisotropic mAnisotropicFiltering;
        const SHADER_SAMPLER_SLOT mSamplerSlot;
    };
}