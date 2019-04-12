#include "Shapes.h"

#include "boost/math/constants/constants.hpp"
#include <array>
#include <cassert>

namespace JonsEngine
{
    void CreateRectangleData(const float sizeX, const float sizeY, const float sizeZ, std::vector<float>& vertexData, std::vector<float>& normalData, std::vector<float>& texcoordData, std::vector<uint16_t>& indiceData)
    {
        const float X = sizeX / 2.0f;
        const float Y = sizeY / 2.0f;
        const float Z = sizeZ / 2.0f;

        std::array<float, 72> cubeVertices = {-X, -Y, -Z, -X, -Y, Z, X, -Y, Z,
                                              X, -Y, -Z, -X, Y, -Z, -X, Y, Z,
                                              X, Y, Z, X, Y, -Z, -X, -Y, -Z,
                                              -X, Y, -Z, X, Y, -Z, X, -Y, -Z,
                                              -X, -Y, Z, -X, Y, Z, X, Y, Z,
                                              X, -Y, Z, -X, -Y, -Z, -X, -Y, Z,
                                              -X, Y, Z, -X, Y, -Z, X, -Y, -Z,
                                              X, -Y, Z, X, Y, Z, X, Y, -Z};

        std::array<float, 72> cubeNormals = {0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
                                             0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                                             0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
                                             0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,
                                             0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                                             0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
                                             -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                                             1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};

        std::array<float, 48> cubeTexCoords = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
                                               0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                                               1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                                               0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
                                               0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
                                               1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f};

        std::array<uint16_t, 36> cubeIndices = {0, 2, 1, 0, 3, 2, 4, 5, 6,
                                                4, 6, 7, 8, 9, 10, 8, 10, 11,
                                                12, 15, 14, 12, 14, 13, 16, 17, 18,
                                                16, 18, 19, 20, 23, 22, 20, 22, 21}; 

        vertexData.insert(vertexData.begin(), cubeVertices.begin(), cubeVertices.begin() + cubeVertices.size());
        normalData.insert(normalData.begin(), cubeNormals.begin(), cubeNormals.begin() + cubeNormals.size());
        texcoordData.insert(texcoordData.begin(), cubeTexCoords.begin(), cubeTexCoords.begin() + cubeTexCoords.size());
        indiceData.insert(indiceData.begin(), cubeIndices.begin(), cubeIndices.begin() + cubeIndices.size());
    }

	void CreatePatchXZ(const float sizeX, const float sizeZ, std::vector<float>& vertexData, std::vector<float>& normalData, std::vector<float>& texcoordData, std::vector<uint16_t>& indiceData)
	{
		
	}

	void CreateSphereData(const float radius, const uint32_t rings, const uint32_t sectors, std::vector<float>& vertexData, std::vector<float>& normalData, std::vector<float>& texcoordData, std::vector<uint16_t>& indiceData)
    {
		assert(radius > 0 && rings > 0 && sectors > 0);

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
    }
}