#pragma once

#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Types.h"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11Texture;

    class DX11SkyboxPass
    {
    public:
        DX11SkyboxPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context);
        ~DX11SkyboxPass();

        void Render(const Mat4& viewMatrix, const Mat4& projMatrix, DX11Texture& skyboxTexture);


    private:
        struct SkyboxCBuffer
        {
            Mat4 mViewMatrix;
            Mat4 mProjMatrix;


            SkyboxCBuffer(const Mat4& viewMatrix, const Mat4& projMatrix) : mViewMatrix(viewMatrix), mProjMatrix(projMatrix)
            {
            }
        };


        ID3D11DeviceContextPtr mContext;
        ID3D11VertexShaderPtr mVertexShader;
        ID3D11PixelShaderPtr mPixelShader;

        DX11ConstantBuffer<SkyboxCBuffer> mSkyboxCBuffer;
    };
}
