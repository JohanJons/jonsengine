#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/Shaders/Constants.h"
#include "include/Core/EngineSettings.h"
#include "include/Core/Types.h"
#include "include/Core/Platform/Directx11.h"

namespace JonsEngine
{
    class DX11Sampler
    {
    public:
        enum SHADER_SAMPLER_SLOT
        {
            SHADER_SAMPLER_SLOT_ANISOTROPIC = SAMPLER_SLOT_ANISOTROPIC,
            SHADER_SAMPLER_SLOT_POINT = SAMPLER_SLOT_POINT,
            SHADER_SAMPLER_SLOT_POINT_COMPARE = SAMPLER_SLOT_POINT_COMPARE
        };
    
        DX11Sampler(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const EngineSettings::Anisotropic maxAnisotropy, const D3D11_FILTER filter, const D3D11_TEXTURE_ADDRESS_MODE addressMode,
            const D3D11_COMPARISON_FUNC comparison, const SHADER_SAMPLER_SLOT samplerSlot);
        ~DX11Sampler();
        
        void BindSampler();
        EngineSettings::Anisotropic GetMaxAnisotropicFiltering() const;
        
    
    private:
        ID3D11DeviceContextPtr mContext;
        ID3D11SamplerStatePtr mTextureSampler;

        const EngineSettings::Anisotropic mAnisotropicFiltering;
        const SHADER_SAMPLER_SLOT mSamplerSlot;
    };
}