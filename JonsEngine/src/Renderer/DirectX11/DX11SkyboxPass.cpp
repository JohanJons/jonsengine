#include "include/Renderer/DirectX11/DX11SkyboxPass.h"

#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/SkyboxVertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/SkyboxPixel.h"

namespace JonsEngine
{
    DX11SkyboxPass::DX11SkyboxPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context) : mContext(context), mVertexShader(nullptr), mPixelShader(nullptr), mSkyboxCBuffer(device, context, mSkyboxCBuffer.CONSTANT_BUFFER_SLOT_VERTEX)
    {
        DXCALL(device->CreateVertexShader(gSkyboxVertexShader, sizeof(gSkyboxVertexShader), nullptr, &mVertexShader));
        DXCALL(device->CreatePixelShader(gSkyboxPixelShader, sizeof(gSkyboxPixelShader), nullptr, &mPixelShader));
    }

    DX11SkyboxPass::~DX11SkyboxPass()
    {
    }


    void DX11SkyboxPass::Render(const Mat4& viewMatrix, const Mat4& projMatrix, DX11Texture& skyboxTexture)
    {
        mSkyboxCBuffer.SetData(SkyboxCBuffer(glm::inverse(viewMatrix), glm::inverse(projMatrix)));
        skyboxTexture.Bind();

        mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        mContext->IASetInputLayout(nullptr);

        mContext->VSSetShader(mVertexShader, nullptr, 0);
        mContext->PSSetShader(mPixelShader, nullptr, 0);

        mContext->Draw(3, 0);
    }
}