#pragma once

#include "include/Core/Types.h"

#include <d3d11.h>

namespace JonsEngine
{
    class GBuffer
    {
    public:
        enum GBUFFER_RENDERTARGET_INDEX
        {
            GBUFFER_RENDERTARGET_INDEX_POSITION = 0,
            GBUFFER_RENDERTARGET_INDEX_DIFFUSE,
            GBUFFER_NUM_RENDERTARGETS
        };


        GBuffer(ID3D11Device* device, IDXGISwapChain* swapChain);
        ~GBuffer();

        void BindForDrawing(ID3D11DeviceContext* context);
        void BindForReading(ID3D11DeviceContext* context);


    private:
        ID3D11Texture2D* mGeometryTextures[GBuffer::GBUFFER_NUM_RENDERTARGETS];
        ID3D11RenderTargetView* mRenderTargets[GBuffer::GBUFFER_NUM_RENDERTARGETS];
        ID3D11Texture2D* mDepthStencilBuffer;
        ID3D11DepthStencilView* mDepthStencilView;
        ID3D11DepthStencilState* mDepthStencilState;
    };
}