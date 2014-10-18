#include "include/Renderer/DirectX11/DX11AmbientPass.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/AmbientPixel.h"

namespace JonsEngine
{
    DX11AmbientPass::DX11AmbientPass(ID3D11DevicePtr device, DX11FullscreenTrianglePass& fullscreenPass) : 
        mFullscreenPass(fullscreenPass), mPixelShader(nullptr), mConstantBuffer(device, mConstantBuffer.CONSTANT_BUFFER_SLOT_PIXEL),
        mSAOTexture1(nullptr), mSAOSRV1(nullptr), mSAORTV1(nullptr), mSAOTexture2(nullptr), mSAOSRV2(nullptr), mSAORTV2(nullptr)
    {
        DXCALL(device->CreatePixelShader(gAmbientPixelShader, sizeof(gAmbientPixelShader), NULL, &mPixelShader));
    }

    DX11AmbientPass::~DX11AmbientPass()
    {
    }


    void DX11AmbientPass::Render(ID3D11DeviceContextPtr context, const Vec4& ambientLight, const bool useSSAO)
    {
      /*  if (useSSAO)
        {
            ID3D11RenderTargetViewPtr prevRTV = nullptr;
            ID3D11DepthStencilViewPtr prevDSV = nullptr;
            context->OMGetRenderTargets(1, &prevRTV, &prevDSV);

            // pass 1: render AO to texture
            context->OMSetRenderTargets(1, &mSAORTV1.p, NULL);
            context->PSSetShader(mSSAOPixelShader, NULL, NULL);
            mSSAOCBuffer.SetData(SSAOCBuffer(camViewProjMatrix, projMatrix, viewMatrix, screenSize), context);
        }*/



        // pass 3: render ambient light
        context->PSSetShader(mPixelShader, NULL, NULL);
        mConstantBuffer.SetData(AmbientCBuffer(ambientLight), context);

        mFullscreenPass.Render(context);
    }
}
