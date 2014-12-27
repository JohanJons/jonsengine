#include "include/Renderer/DirectX11/DX11Sampler.h"

namespace JonsEngine
{
    DX11Sampler::DX11Sampler(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const EngineSettings::Anisotropic maxAnisotropy, const D3D11_FILTER filter, const D3D11_COMPARISON_FUNC comparison, const SHADER_SAMPLER_SLOT samplerSlot) :
        mContext(context), mTextureSampler(nullptr), mAnisotropicFiltering(maxAnisotropy), mSamplerSlot(samplerSlot)
    {
        D3D11_SAMPLER_DESC samplerDesc;
        ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
        samplerDesc.Filter = filter;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.MaxAnisotropy = mAnisotropicFiltering;
        samplerDesc.ComparisonFunc = comparison;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
        DXCALL(device->CreateSamplerState(&samplerDesc, &mTextureSampler));
    }
    
    DX11Sampler::~DX11Sampler()
    {
    }
    
    
    void DX11Sampler::BindSampler()
    {
        mContext->PSSetSamplers(mSamplerSlot, 1, &mTextureSampler.p);
    }
    
    EngineSettings::Anisotropic DX11Sampler::GetMaxAnisotropicFiltering() const
    {
        return mAnisotropicFiltering;
    }
}