#pragma once

#include "include/Core/Types.h"
#include "include/Renderer/IVertexBuffer.h"

namespace JonsEngine
{
    /* RenderItem definition */
    struct RenderItem
    {
        VertexBufferPtr mVertexBuffer;
        const Mat4 mMVP;


        RenderItem(VertexBufferPtr vertexBuffer, const Mat4& mvp);
    };


    /* RenderItem inlines */
    inline RenderItem::RenderItem(VertexBufferPtr vertexBuffer, const Mat4& mvp) : mVertexBuffer(vertexBuffer), mMVP(mvp)
    {
    }
}