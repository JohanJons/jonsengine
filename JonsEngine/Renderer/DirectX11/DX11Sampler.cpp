#include "DX11Sampler.h"

namespace JonsEngine
{
    DX11Sampler::DX11Sampler(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const RenderSettings::Anisotropic maxAnisotropy, const D3D11_FILTER filter, const D3D11_TEXTURE_ADDRESS_MODE addressMode,
        const D3D11_COMPARISON_FUNC comparison, const SHADER_SAMPLER_SLOT samplerSlot) 
        :
        DX11Sampler(device, context, maxAnisotropy, filter, addressMode, addressMode, addressMode, comparison, samplerSlot)
    {
    }
    
    DX11Sampler::DX11Sampler(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const RenderSettings::Anisotropic maxAnisotropy, const D3D11_FILTER filter, const D3D11_TEXTURE_ADDRESS_MODE addressModeU,
        const D3D11_TEXTURE_ADDRESS_MODE addressModeV, const D3D11_TEXTURE_ADDRESS_MODE addressModeW, const D3D11_COMPARISON_FUNC comparison, const SHADER_SAMPLER_SLOT samplerSlot)
        :
        mContext(context), mTextureSampler(nullptr), mAnisotropicFiltering(maxAnisotropy), mSamplerSlot(samplerSlot)
    {
        D3D11_SAMPLER_DESC samplerDesc;
        ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
        samplerDesc.Filter = filter;
        samplerDesc.AddressU = addressModeU;
        samplerDesc.AddressV = addressModeV;
        samplerDesc.AddressW = addressModeW;
        samplerDesc.MaxAnisotropy = RenderSettingsToVal(mAnisotropicFiltering);
        samplerDesc.ComparisonFunc = comparison;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
        DXCALL(device->CreateSamplerState(&samplerDesc, &mTextureSampler));
    }

    DX11Sampler::~DX11Sampler()
    {
    }
    
    
    void DX11Sampler::BindSampler()
    {
		mContext->VSSetSamplers( mSamplerSlot, 1, &mTextureSampler.p );
        mContext->PSSetSamplers( mSamplerSlot, 1, &mTextureSampler.p );
		mContext->HSSetSamplers( mSamplerSlot, 1, &mTextureSampler.p );
		mContext->DSSetSamplers( mSamplerSlot, 1, &mTextureSampler.p );
    }
    
	RenderSettings::Anisotropic DX11Sampler::GetMaxAnisotropicFiltering() const
    {
        return mAnisotropicFiltering;
    }
}