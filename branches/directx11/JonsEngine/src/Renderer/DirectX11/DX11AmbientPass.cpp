#include "include/Renderer/DirectX11/DX11AmbientPass.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/FullscreenTriangleVertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/AmbientPixel.h"

namespace JonsEngine
{
    DX11AmbientPass::DX11AmbientPass(ID3D11Device* device) : mVertexShader(nullptr), mPixelShader(nullptr), mConstantBuffer(device)
    {
        DXCALL(device->CreateVertexShader(gFullscreenTriangleVertexShader, sizeof(gFullscreenTriangleVertexShader), NULL, &mVertexShader));
        DXCALL(device->CreatePixelShader(gAmbientPixelShader, sizeof(gAmbientPixelShader), NULL, &mPixelShader));
    }

    DX11AmbientPass::~DX11AmbientPass()
    {
        mVertexShader->Release();
        mPixelShader->Release();
    }


    void DX11AmbientPass::Render(ID3D11DeviceContext* context, const Vec4& ambientLight)
    {
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        for (uint32_t index = 0; index < DX11Mesh::NUM_VERTEX_BUFFER_SLOTS; index++)
            context->IASetVertexBuffers(index, 0, NULL, 0, 0);
        context->IASetInputLayout(NULL);

        context->VSSetShader(mVertexShader, NULL, NULL);
        context->PSSetShader(mPixelShader, NULL, NULL);

        mConstantBuffer.SetData(AmbientCBuffer(ambientLight), context, 0);

        context->Draw(3, 0);
    }
}