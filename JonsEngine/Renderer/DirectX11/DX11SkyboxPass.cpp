#include "Renderer/DirectX11/DX11SkyboxPass.h"

#include "Renderer/DirectX11/DX11Texture.h"
#include "Compiled/SkyboxVertex.h"
#include "Compiled/SkyboxPixel.h"

namespace JonsEngine
{
    DX11SkyboxPass::DX11SkyboxPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context) : mContext(context), mVertexShader(nullptr), mPixelShader(nullptr)
    {
        DXCALL(device->CreateVertexShader(gSkyboxVertex, sizeof(gSkyboxVertex), nullptr, &mVertexShader));
        DXCALL(device->CreatePixelShader(gSkyboxPixel, sizeof(gSkyboxPixel), nullptr, &mPixelShader));
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