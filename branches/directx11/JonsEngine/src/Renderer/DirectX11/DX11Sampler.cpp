#include "include/Renderer/DirectX11/DX11Sampler.h"

namespace JonsEngine
{
    DX11Sampler::DX11Sampler(ID3D11DevicePtr device, const EngineSettings::Anisotropic maxAnisotropy, const D3D11_FILTER filter, const SHADER_SAMPLER_SLOT samplerSlot) : 
        mTextureSampler(nullptr), mAnisotropicFiltering(maxAnisotropy), mSamplerSlot(samplerSlot)
    {
        D3D11_SAMPLER_DESC samplerDesc;
        ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
        samplerDesc.Filter = filter;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.MaxAnisotropy = mAnisotropicFiltering;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
        DXCALL(mDevice->CreateSamplerState(&samplerDesc, &mShadowmapSampler));
    }
    
    DX11Sampler::~DX11Sampler()
    {
    }
    
    
    void DX11Sampler::BindSampler(ID3D11DeviceContextPtr context)
    {
        context->PSSetSamplers(mSamplerSlot, 1, &mTextureSampler.p);
    }
    
    void DX11Sampler::GetMaxAnisotropicFiltering() const
    {
        return mAnisotropicFiltering;
    }
}