#include "include/Renderer/DirectX11/DX11VertexTransformPass.h"

#include "include/Renderer/DirectX11/Shaders/Compiled/TransformVertex.h"

namespace JonsEngine
{
    DX11VertexTransformPass::DX11VertexTransformPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, IDMap<DX11Mesh>& meshMap) :
        mContext(context), mTransformCBuffer(device, context, mTransformCBuffer.CONSTANT_BUFFER_SLOT_VERTEX), mMeshMap(meshMap)
    {
        D3D11_INPUT_ELEMENT_DESC inputDescription;
        ZeroMemory(&inputDescription, sizeof(D3D11_INPUT_ELEMENT_DESC));
        inputDescription.SemanticName = "POSITION";
        inputDescription.SemanticIndex = 0;
        inputDescription.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputDescription.InputSlot = 0;
        inputDescription.AlignedByteOffset = 0;
        inputDescription.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription.InstanceDataStepRate = 0;
        DXCALL(device->CreateInputLayout(&inputDescription, 1, gTransformVertexShader, sizeof(gTransformVertexShader), &mInputLayout));

        DXCALL(device->CreateVertexShader(gTransformVertexShader, sizeof(gTransformVertexShader), nullptr, &mVertexShader));
    }

    DX11VertexTransformPass::~DX11VertexTransformPass()
    {
    }


    void DX11VertexTransformPass::BindForTransformPass(const D3D_PRIMITIVE_TOPOLOGY primitiveTopology)
    {
        mContext->IASetPrimitiveTopology(primitiveTopology);
        mContext->IASetInputLayout(mInputLayout);

        mContext->VSSetShader(mVertexShader, nullptr, 0);
    }

    void DX11VertexTransformPass::RenderMesh(DX11Mesh& mesh, const Mat4& wvpMatrix)
    {
        mTransformCBuffer.SetData(TransformCBuffer(wvpMatrix));
        mesh.DrawPositions();
    }

    template <typename T>
    void RenderMeshesAux(const T& meshContainer, const IDMap<Mat4>& localTransformStorage, const IDMap<Mat4>& worldTransformStorage, const Mat4& viewProjectionMatrix)
    {
        for (const RenderableMesh& mesh : meshContainer)
        {
            const Mat4& localTransform = localTransformStorage.GetItem(mesh.mLocalTransformID);
            const Mat4& worldTransform = worldTransformStorage.GetItem(mesh.mWorldTransformID);

            mTransformCBuffer.SetData(TransformCBuffer(viewProjectionMatrix * worldTransform * localTransform));
            mMeshMap.GetItem(mesh.mMeshID).DrawPositions();
        }
    }

    void DX11VertexTransformPass::RenderMeshes(const RenderableMeshes& meshes, const IDMap<Mat4>& localTransformStorage, const IDMap<Mat4>& worldTransformStorage, const Mat4& viewProjectionMatrix)
    {
        RenderMeshesAux(meshes, localTransformStorage, worldTransformStorage, viewProjectionMatrix);
    }

    void DX11VertexTransformPass::RenderMeshes(ConstRangedIterator<RenderableMeshes>& meshIterator, const IDMap<Mat4>& localTransformStorage, const IDMap<Mat4>& worldTransformStorage, const Mat4& viewProjectionMatrix)
    {
        RenderMeshesAux(meshIterator, localTransformStorage, worldTransformStorage, viewProjectionMatrix);
    }

    void DX11VertexTransformPass::RenderAABBs(const RenderableModels& models, const IDMap<Mat4>& localTransformStorage, const IDMap<Mat4>& worldTransformStorage, const Mat4& viewProjectionMatrix)
    {
        for (const RenderableModel& model : models)
        {
            const Mat4& localTransform = localTransformStorage.GetItem(model.mMesh.mLocalTransformID);
            const Mat4& worldTransform = worldTransformStorage.GetItem(model.mMesh.mWorldTransformID);

            mTransformCBuffer.SetData(TransformCBuffer(viewProjectionMatrix * worldTransform * localTransform));
            mMeshMap.GetItem(model.mMesh.mMeshID).DrawAABB();
        }
    }
}