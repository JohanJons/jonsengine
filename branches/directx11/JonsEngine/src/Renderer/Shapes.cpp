#include "include/Renderer/Shapes.h"

#include "boost/math/constants/constants.hpp"

namespace JonsEngine
{
    bool CreateRectangleData(const float sizeX, const float sizeY, const float sizeZ, std::vector<float>& vertexData, std::vector<float>& normalData, std::vector<float>& texcoordData, std::vector<uint32_t>& indiceData)
    {
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
        /*float cubeVertices[] = { 0.0f, 0.5f, 0.0f,
                                 0.45f, -0.5, 0.0f,
                                 -0.45f, -0.5f, 0.0f };
        uint32_t cubeIndices[] = {0, 2, 1};*/

        vertexData.insert(vertexData.begin(), cubeVertices, cubeVertices + (sizeof(cubeVertices) / sizeof(float)));
        normalData.insert(normalData.begin(), cubeNormals, cubeNormals + (sizeof(cubeNormals) / sizeof(float)));
        texcoordData.insert(texcoordData.begin(), cubeTexCoords, cubeTexCoords + (sizeof(cubeTexCoords) / sizeof(float)));
        indiceData.insert(indiceData.begin(), cubeIndices, cubeIndices + (sizeof(cubeIndices) / sizeof(uint32_t)));

        return true;
    }

    bool CreateSphereData(const float radius, const uint32_t rings, const uint32_t sectors, std::vector<float>& vertexData, std::vector<float>& normalData, std::vector<float>& texcoordData, std::vector<uint32_t>& indiceData)
    {
        if (radius <= 0 || rings <= 0 || sectors <= 0)
            return false;

        const float R = 1.0f / (float)(rings - 1);
        const float S = 1.0f / (float)(sectors - 1);
        const float pi = boost::math::constants::pi<float>();
        const float pi_2 = pi / 2;

        vertexData.resize(rings * sectors * 3);
        normalData.resize(rings * sectors * 3);
        texcoordData.resize(rings * sectors * 2);
        auto v = vertexData.begin();
        auto n = normalData.begin();
        auto t = texcoordData.begin();
        for (uint32_t r = 0; r < rings; r++)
        {
            for (uint32_t s = 0; s < sectors; s++)
            {
                const float y = sin(-pi_2 + pi * r * R);
                const float x = cos(2 * pi * s * S) * sin(pi * r * R);
                const float z = sin(2 * pi * s * S) * sin(pi * r * R);

                *t++ = s*S;
                *t++ = r*R;

                *v++ = x * radius;
                *v++ = y * radius;
                *v++ = z * radius;

                *n++ = x;
                *n++ = y;
                *n++ = z;
            }
        }

        indiceData.resize(rings * sectors * 6);
        auto i = indiceData.begin();
        for (uint32_t r = 0; r < rings-1; r++)
        {
            for (uint32_t s = 0; s < sectors-1; s++)
            {
                *i++ = r * sectors + (s + 1);
                *i++ = r * sectors + s;
                *i++ = (r + 1) * sectors + s;

                *i++ = r * sectors + (s + 1);
                *i++ = (r + 1) * sectors + s;
                *i++ = (r + 1) * sectors + (s + 1);
            }
        }

        return true;
    }
}