#pragma once

#include "Renderer/DirectX11/DX11Utils.h"
#include "Renderer/DirectX11/DX11Buffer.hpp"
#include "Core/Types.h"
#include "Core/Platform.h"

#include <vector>

namespace JonsEngine
{
    class DX11VertexBufferSet
    {
    public:
        template <typename... T_ARGS>
        DX11VertexBufferSet(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, T_ARGS&&... args);
        ~DX11VertexBufferSet();

        void Bind(const uint32_t numBuffers);


    private:
        template <typename... T_ARGS>
        void AddVertexBuffers(ID3D11DevicePtr device, T_ARGS&&... args);
        template <typename T, typename... T_ARGS>
        void AddVertexBuffers(ID3D11DevicePtr device, const std::vector<T>& data, const uint32_t stride, T_ARGS&&... args);


        ID3D11DeviceContextPtr mContext;
        std::vector<ID3D11Buffer> mBuffers;
        std::vector<uint32_t> mStrides;
    };



    template <typename... T_ARGS>
    DX11VertexBufferSet::DX11VertexBufferSet(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, T_ARGS&&... args) :
        mContext(context)
    {
        AddVertexBuffers(device, args...);
    }


    template <typename... T_ARGS>
    void DX11VertexBufferSet::AddVertexBuffers(ID3D11DevicePtr device, T_ARGS&&... args)
    {
    }

    template <typename T, typename... T_ARGS>
    void DX11VertexBufferSet::AddVertexBuffers(ID3D11DevicePtr device, const std::vector<T>& data, const uint32_t stride, T_ARGS&&... args)
    {
        if (data.size() > 0)
        {
            mStrides.emplace_back(stride);

            mBuffers.emplace_back();
            ID3D11Buffer& buffer = mBuffers.back();

            D3D11_BUFFER_DESC bufferDescription;
            ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
            bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
            bufferDescription.ByteWidth = data.size() * sizeof(T);
            bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

            D3D11_SUBRESOURCE_DATA initData;
            ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
            initData.pSysMem = &data.at(0);
            DXCALL(device->CreateBuffer(&bufferDescription, &initData, &buffer));
        }

        AddVertexBuffers(device, args...);
    }
}