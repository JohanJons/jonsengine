#pragma once

#include "include/Renderer/IVertexBuffer.h"

#include "boost/smart_ptr.hpp"

namespace JonsEngine
{
    class IRenderer;
    class SceneNode;
    class Mesh;

    /* MeshPtr definition */
    typedef boost::shared_ptr<Mesh> MeshPtr;
   
    /* Mesh defintion */
    class Mesh
    {
    public:
        static MeshPtr CreateMesh(const std::string& name, VertexBufferPtr vertexBuffer);
        ~Mesh();

        VertexBufferPtr GetVertexBuffer();

        bool operator==(const Mesh& s1);
        bool operator==(const std::string& meshName);

    private:
        VertexBufferPtr mVertexBuffer;
        std::string mName;
        size_t mHashedID;
    };
}