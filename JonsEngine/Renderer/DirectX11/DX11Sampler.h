#pragma once

#include "Renderer/DirectX11/DX11Utils.h"
#include "Renderer/DirectX11/Shaders/Constants.h"
#include "Core/EngineSettings.h"
#include "Core/Types.h"
#include "Core/Platform.h"

namespace JonsEngine
{
    class DX11Sampler
    {
    public:
        enum SHADER_SAMPLER_SLOT
        {
            SHADER_SAMPLER_SLOT_ANISOTROPIC = SAMPLER_SLOT_ANISOTROPIC,
            SHADER_SAMPLER_SLOT_POINT = SAMPLER_SLOT_POINT,
            SHADER_SAMPLER_SLOT_POINT_COMPARE = SAMPLER_SLOT_POINT_COMPARE,
            SHADER_SAMPLER_SLOT_LINEAR = SAMPLER_SLOT_LINEAR,
			SHADER_SAMPLER_SLOT_LINEAR_WRAP = SAMPLER_SLOT_LINEAR_WRAP,
        };
    
        DX11Sampler(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const RenderSettings::Anisotropic maxAnisotropy, const D3D11_FILTER filter, const D3D11_TEXTURE_ADDRESS_MODE addressMode,
            const D3D11_COMPARISON_FUNC comparison, const SHADER_SAMPLER_SLOT samplerSlot);
        DX11Sampler(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const RenderSettings::Anisotropic maxAnisotropy, const D3D11_FILTER filter, const D3D11_TEXTURE_ADDRESS_MODE addressModeU,
            const D3D11_TEXTURE_ADDRESS_MODE addressModeV, const D3D11_TEXTURE_ADDRESS_MODE addressModeW, const D3D11_COMPARISON_FUNC comparison, const SHADER_SAMPLER_SLOT samplerSlot);
        ~DX11Sampler();
        
        void BindSampler();
		RenderSettings::Anisotropic GetMaxAnisotropicFiltering() const;
        
    
    private:
        ID3D11DeviceContextPtr mContext;
        ID3D11SamplerStatePtr mTextureSampler;

        const RenderSettings::Anisotropic mAnisotropicFiltering;
        const SHADER_SAMPLER_SLOT mSamplerSlot;
    };
}