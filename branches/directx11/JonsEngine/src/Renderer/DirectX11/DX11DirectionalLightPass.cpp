#include "include/Renderer/DirectX11/DX11DirectionalLightPass.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/FullscreenTriangleVertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/DirectionalLightPixel.h"

namespace JonsEngine
{
    DX11DirectionalLightPass::DX11DirectionalLightPass(ID3D11Device* device) : mVertexShader(nullptr), mPixelShader(nullptr), mConstantBuffer(device)
    {
        DXCALL(device->CreateVertexShader(gFullscreenTriangleVertexShader, sizeof(gFullscreenTriangleVertexShader), NULL, &mVertexShader));
        DXCALL(device->CreatePixelShader(gDirectionalLightPixelShader, sizeof(gDirectionalLightPixelShader), NULL, &mPixelShader));
    }

    DX11DirectionalLightPass::~DX11DirectionalLightPass()
    {
        mVertexShader->Release();
        mPixelShader->Release();
    }


    void DX11DirectionalLightPass::BindForDrawing(ID3D11DeviceContext* context)
    {
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        for (uint32_t index = 0; index < DX11Mesh::NUM_VERTEX_BUFFER_SLOTS; index++)
            context->IASetVertexBuffers(index, 0, NULL, 0, 0);
        context->IASetInputLayout(NULL);

        context->VSSetShader(mVertexShader, NULL, NULL);
        context->PSSetShader(mPixelShader, NULL, NULL);
    }

    void DX11DirectionalLightPass::Render(ID3D11DeviceContext* context, const Vec4& lightColor, const Vec3& lightDir, uint32_t screenWidth, uint32_t screenHeight)
    {
        mConstantBuffer.SetData(DirectionalLightCBuffer(lightColor, lightDir, Vec2(screenWidth, screenHeight)), context, 0);

        context->Draw(3, 0);
    }
}