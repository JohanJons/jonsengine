#include "include/Renderer/Shapes.h"

namespace JonsEngine
{
    bool CreateRectangleData(const float sizeX, const float sizeY, const float sizeZ, std::vector<float>& vertexData, std::vector<float>& normalData, std::vector<float>& texcoordData, std::vector<uint32_t>& indiceData)
    {
        if (sizeX <= 0 || sizeY <= 0 || sizeZ <= 0)
            return false;
        
        const float X = sizeX / 2.0f;
        const float Y = sizeY / 2.0f;
        const float Z = sizeZ / 2.0f;

        float cubeVertices[] = {-X, -Y, -Z, -X, -Y, Z, X, -Y, Z,
                                X, -Y, -Z, -X, Y, -Z, -X, Y, Z,
                                X, Y, Z, X, Y, -Z, -X, -Y, -Z,
                                -X, Y, -Z, X, Y, -Z, X, -Y, -Z,
                                -X, -Y, Z, -X, Y, Z, X, Y, Z,
                                X, -Y, Z, -X, -Y, -Z, -X, -Y, Z,
                                -X, Y, Z, -X, Y, -Z, X, -Y, -Z,
                                X, -Y, Z, X, Y, Z, X, Y, -Z};

        float cubeNormals[] = {0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
                               0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                               0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
                               0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,
                               0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                               0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
                               -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                               1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};

        float cubeTexCoords[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
                                 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                                 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                                 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
                                 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
                                 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f};

        uint32_t cubeIndices[] = {0, 2, 1, 0, 3, 2, 4, 5, 6,
                                  4, 6, 7, 8, 9, 10, 8, 10, 11,
                                  12, 15, 14, 12, 14, 13, 16, 17, 18,
                                  16, 18, 19, 20, 23, 22, 20, 22, 21};

        vertexData.insert(vertexData.begin(), cubeVertices, cubeVertices + (sizeof(cubeVertices) / sizeof(float)));
        normalData.insert(normalData.begin(), cubeNormals, cubeNormals + (sizeof(cubeNormals) / sizeof(float)));
        texcoordData.insert(texcoordData.begin(), cubeTexCoords, cubeTexCoords + (sizeof(cubeTexCoords) / sizeof(float)));
        indiceData.insert(indiceData.begin(), cubeIndices, cubeIndices + (sizeof(cubeIndices) / sizeof(uint32_t)));

        return true;
    }
}