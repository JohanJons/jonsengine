#include "include/Scene/Mesh.h"

#include "include/Renderer/IRenderer.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Scene/SceneNode.h"

#include "boost/bind.hpp"
#include "boost/functional/hash.hpp"

namespace JonsEngine
{
    MeshPtr Mesh::CreateMesh(const std::string& name, VertexBufferPtr vertexBuffer)
    {
        MeshPtr ptr((Mesh*)HeapAllocator::GetDefaultHeapAllocator().Allocate(sizeof(Mesh)), boost::bind(&HeapAllocator::DeallocateObject<Mesh>, &HeapAllocator::GetDefaultHeapAllocator(), _1));
        
        ptr->mName = name;
        ptr->mHashedID = boost::hash_value(name);
        ptr->mVertexBuffer = vertexBuffer;

        return ptr;
    }
        
    Mesh::~Mesh()
    {
    }

        
    VertexBufferPtr Mesh::GetVertexBuffer()
    {
        return mVertexBuffer;
    }


    bool Mesh::operator==(const Mesh& s1)
    {
        return mHashedID == s1.mHashedID;
    }
        
    bool Mesh::operator==(const std::string& meshName)
    {
        return mHashedID == boost::hash_value(meshName);
    }
}