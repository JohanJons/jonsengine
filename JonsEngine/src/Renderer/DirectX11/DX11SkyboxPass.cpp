#include "include/Renderer/DirectX11/DX11SkyboxPass.h"

#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/SkyboxVertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/SkyboxPixel.h"

namespace JonsEngine
{
    DX11SkyboxPass::DX11SkyboxPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context) : mContext(context), mVertexShader(nullptr), mPixelShader(nullptr)
    {
        DXCALL(device->CreateVertexShader(gSkyboxVertexShader, sizeof(gSkyboxVertexShader), nullptr, &mVertexShader));
        DXCALL(device->CreatePixelShader(gSkyboxPixelShader, sizeof(gSkyboxPixelShader), nullptr, &mPixelShader));
    }

    DX11SkyboxPass::~DX11SkyboxPass()
    {
    }


    void DX11SkyboxPass::Render(DX11Texture& skyboxTexture)
    {
        skyboxTexture.BindAsShaderResource(SHADER_TEXTURE_SLOT_EXTRA);

        mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        mContext->IASetInputLayout(nullptr);

        mContext->VSSetShader(mVertexShader, nullptr, 0);
        mContext->PSSetShader(mPixelShader, nullptr, 0);

        mContext->Draw(3, 0);
    }
}