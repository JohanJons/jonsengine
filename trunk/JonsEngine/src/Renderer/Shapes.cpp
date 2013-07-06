#include "include/Renderer/Shapes.h"

namespace JonsEngine
{
    bool CreateRectangleData(const double sizeX, const double sizeY, const double sizeZ, std::vector<Vec3>& vertexData, std::vector<Vec3>& normalData, std::vector<uint32_t>& indiceData)
    {
        const double X = sizeX / 2.0f;
        const double Y = sizeY / 2.0f;
        const double Z = sizeZ / 2.0f;


        Vec3 cubeVertices[] = {Vec3(-X, -Y, -Z), Vec3(-X, -Y, Z), Vec3(X, -Y, Z), Vec3(X, -Y, -Z),
                               Vec3(-X, Y, -Z), Vec3(-X, Y, Z), Vec3(X, Y, Z), Vec3(X, Y, -Z),
                               Vec3(-X, -Y, -Z), Vec3(-X, Y, -Z), Vec3(X, Y, -Z), Vec3(X, -Y, -Z),
                               Vec3(-X, -Y, Z), Vec3(-X, Y, Z), Vec3(X, Y, Z), Vec3(X, -Y, Z),
                               Vec3(-X, -Y, -Z), Vec3(-X, -Y, Z), Vec3(-X, Y, Z), Vec3(-X, Y, -Z),
                               Vec3(X, -Y, -Z), Vec3(X, -Y, Z), Vec3(X, Y, Z), Vec3(X, Y, -Z)};

        float cubeNormals[] = {  0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,

    0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f,

    0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,

    0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f,

    -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

    +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f };

        return true;
    }
}