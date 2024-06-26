#include "Chunk.h"
#include "VoxelRenderer.h"
#include <vector>
#include <glm/common.hpp>
#include <chrono>
#include <iostream>

struct Vertex
{
    glm::vec3 pos;
    uint8_t normal; // samo indeks
    glm::u8vec3 color; // 3 bytes
};

constexpr auto r = 1.0f / 16.0f / 2.0f;

namespace voxr
{
    Chunk::Chunk()
    {
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
        glEnableVertexAttribArray(1);
        glVertexAttribIPointer(1, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribIPointer(2, 3, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(Vertex, color));

        m_voxels = new Voxel[width * width * width];
        assert(m_voxels != nullptr && "Failed to allocate voxels for a chunk!");
    }

    Chunk::~Chunk()
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);

        delete[] m_voxels;
    }

    void AddFaceBottom(const glm::vec3& center, std::vector<Vertex>& vertices, const glm::vec3& color)
    {
        Vertex v1;
        v1.pos = center + glm::vec3(-r, -r, -r);
        Vertex v2;
        v2.pos = center + glm::vec3(r, -r, -r);
        Vertex v3;
        v3.pos = center + glm::vec3(r, -r, r);
        Vertex v4;
        v4.pos = center + glm::vec3(-r, -r, r);

        //v1.normal = v2.normal = v3.normal = v4.normal = glm::vec3(0, -1, 0);
        v1.normal = v2.normal = v3.normal = v4.normal = 2;
        v1.color = v2.color = v3.color = v4.color = color;

        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);

        vertices.push_back(v3);
        vertices.push_back(v4);
        vertices.push_back(v1);
    }
    
    void AddFaceTop(const glm::vec3& center, std::vector<Vertex>& vertices, const glm::vec3& color)
    {
        Vertex v1;
        v1.pos = center + glm::vec3(-r, r, -r);
        Vertex v2;
        v2.pos = center + glm::vec3(r, r, -r);
        Vertex v3;
        v3.pos = center + glm::vec3(r, r, r);
        Vertex v4;
        v4.pos = center + glm::vec3(-r, r, r);

        //v1.normal = v2.normal = v3.normal = v4.normal = glm::vec3(0, 1, 0);
        v1.normal = v2.normal = v3.normal = v4.normal = 3;
        v1.color = v2.color = v3.color = v4.color = color;

        vertices.push_back(v3);
        vertices.push_back(v2);
        vertices.push_back(v1);

        vertices.push_back(v1);
        vertices.push_back(v4);
        vertices.push_back(v3);
    }
    
    void AddFaceLeft(const glm::vec3& center, std::vector<Vertex>& vertices, const glm::vec3& color)
    {
        Vertex v1;
        v1.pos = center + glm::vec3(-r, -r, -r);
        Vertex v2;
        v2.pos = center + glm::vec3(-r, -r, r);
        Vertex v3;
        v3.pos = center + glm::vec3(-r, r, r);
        Vertex v4;
        v4.pos = center + glm::vec3(-r, r, -r);

        //v1.normal = v2.normal = v3.normal = v4.normal = glm::vec3(-1, 0, 0);
        v1.normal = v2.normal = v3.normal = v4.normal = 0;
        v1.color = v2.color = v3.color = v4.color = color;

        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);

        vertices.push_back(v3);
        vertices.push_back(v4);
        vertices.push_back(v1);
    }

    void AddFaceRight(const glm::vec3& center, std::vector<Vertex>& vertices, const glm::vec3& color)
    {
        Vertex v1;
        v1.pos = center + glm::vec3(r, -r, -r);
        Vertex v2;
        v2.pos = center + glm::vec3(r, -r, r);
        Vertex v3;
        v3.pos = center + glm::vec3(r, r, r);
        Vertex v4;
        v4.pos = center + glm::vec3(r, r, -r);

        //v1.normal = v2.normal = v3.normal = v4.normal = glm::vec3(1, 0, 0);
        v1.normal = v2.normal = v3.normal = v4.normal = 1;
        v1.color = v2.color = v3.color = v4.color = color;

        vertices.push_back(v3);
        vertices.push_back(v2);
        vertices.push_back(v1);

        vertices.push_back(v1);
        vertices.push_back(v4);
        vertices.push_back(v3);
    }

    void AddFaceFront(const glm::vec3& center, std::vector<Vertex>& vertices, const glm::vec3& color)
    {
        Vertex v1;
        v1.pos = center + glm::vec3(-r, -r, r);
        Vertex v2;
        v2.pos = center + glm::vec3(r, -r, r);
        Vertex v3;
        v3.pos = center + glm::vec3(r, r, r);
        Vertex v4;
        v4.pos = center + glm::vec3(-r, r, r);

        //v1.normal = v2.normal = v3.normal = v4.normal = glm::vec3(0, 0, 1);
        v1.normal = v2.normal = v3.normal = v4.normal = 5;
        v1.color = v2.color = v3.color = v4.color = color;

        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);

        vertices.push_back(v3);
        vertices.push_back(v4);
        vertices.push_back(v1);
    }

    void AddFaceBack(const glm::vec3& center, std::vector<Vertex>& vertices, const glm::vec3& color)
    {
        Vertex v1;
        v1.pos = center + glm::vec3(-r, -r, -r);
        Vertex v2;
        v2.pos = center + glm::vec3(r, -r, -r);
        Vertex v3;
        v3.pos = center + glm::vec3(r, r, -r);
        Vertex v4;
        v4.pos = center + glm::vec3(-r, r, -r);

        //v1.normal = v2.normal = v3.normal = v4.normal = glm::vec3(0, 0, -1);
        v1.normal = v2.normal = v3.normal = v4.normal = 4;
        v1.color = v2.color = v3.color = v4.color = color;

        vertices.push_back(v3);
        vertices.push_back(v2);
        vertices.push_back(v1);

        vertices.push_back(v1);
        vertices.push_back(v4);
        vertices.push_back(v3);
    }

    void Chunk::GenerateMesh()
    {
        std::vector<Vertex> vertices;
        vertices.reserve(m_numVertices + 36);

        srand(69u);

        for (int y = width - 1; y >= 0; y--)
        {
            for (int z = 0; z < width; z++)
            {
                for (int x = 0; x < width; x++)
                {
                    float frand = (float)rand() / RAND_MAX;

                    voxr::Voxel voxel = GetVoxel(x, y, z);

                    if (voxel == Voxel::Air)
                        continue;

                    glm::vec3 center = {
                        (x - width / 2.0f) * 1.0f / 16.0f,
                        (y - width / 2.0f) * 1.0f / 16.0f,
                        (z - width / 2.0f) * 1.0f / 16.0f
                    };

                    glm::u8vec3 color;

                    if (voxel == Voxel::Grass)
                    {
                        constexpr glm::vec3 colorA = glm::vec3(216, 245, 86);
                        constexpr glm::vec3 colorB = glm::vec3(50, 191, 13);
                        color = glm::mix(colorA, colorB, frand);
                    }
                    else if (voxel == Voxel::Sand)
                    {
                        constexpr glm::vec3 colorA = glm::vec3(247, 227, 7);
                        constexpr glm::vec3 colorB = glm::vec3(255, 242, 97);
                        color = glm::mix(colorA, colorB, frand);
                    }
                    else if (voxel == Voxel::Water)
                    {
                        constexpr glm::vec3 colorA = glm::vec3(37, 162, 245);
                        constexpr glm::vec3 colorB = glm::vec3(26, 130, 199);
                        color = glm::mix(colorA, colorB, frand);
                    }
                    else if (voxel == Voxel::Wood)
                    {
                        constexpr glm::vec3 colorA = glm::vec3(138, 79, 10);
                        constexpr glm::vec3 colorB = glm::vec3(105, 58, 2);
                        color = glm::mix(colorA, colorB, frand);
                    }
                    else if (voxel == Voxel::Leaf)
                    {
                        constexpr glm::vec3 colorA = glm::vec3(29, 173, 69);
                        constexpr glm::vec3 colorB = glm::vec3(14, 227, 72);
                        color = glm::mix(colorA, colorB, frand);
                    }
                    

                    if (y == 0 || GetVoxel(x, y - 1, z) == Voxel::Air)
                        AddFaceBottom(center, vertices, color);

                    if (y == width - 1 || GetVoxel(x, y + 1, z) == Voxel::Air)
                        AddFaceTop(center, vertices, color);

                    if (x == 0 || GetVoxel(x - 1, y, z) == Voxel::Air)
                        AddFaceLeft(center, vertices, color);

                    if (x == width - 1 || GetVoxel(x + 1, y, z) == Voxel::Air)
                        AddFaceRight(center, vertices, color);

                    if (z == width - 1 || GetVoxel(x, y, z + 1) == Voxel::Air)
                        AddFaceFront(center, vertices, color);

                    if (z == 0 || GetVoxel(x, y, z - 1) == Voxel::Air)
                        AddFaceBack(center, vertices, color);
                }
            }
        }

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
        
        m_numVertices = vertices.size();
    }
}

