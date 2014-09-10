#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Types.h"

#include <d3d11.h>
#include <vector>

namespace JonsEngine
{
    class DX11Shadowmap
    {
    public:
        DX11Shadowmap(ID3D11DevicePtr device, const uint32_t shadowmapSize, const uint32_t numTextures, const bool isCubeTexture);
        ~DX11Shadowmap();

        void BindForDrawing(ID3D11DeviceContextPtr context);
        void BindDepthView(ID3D11DeviceContextPtr context, const uint32_t depthViewIndex);
        void BindForReading(ID3D11DeviceContextPtr context);

        uint32_t GetTextureSize() const;


    private:
        ID3D11Texture2DPtr mShadowmapTexture;
        ID3D11InputLayoutPtr mInputLayout;
        std::vector<ID3D11DepthStencilViewPtr> mShadowmapViews;
        ID3D11ShaderResourceViewPtr mShadowmapSRV;
        D3D11_VIEWPORT mShadowPassViewport;
    };
}