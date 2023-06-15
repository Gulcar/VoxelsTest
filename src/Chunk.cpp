#include "Chunk.h"
#include "VoxelRenderer.h"
#include <vector>
#include <glm/common.hpp>
#include <chrono>
#include <iostream>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 color;
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
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

        m_voxels = new Voxel[width * width * width];
    }

    Chunk::~Chunk()
    {
        //glDeleteVertexArrays(1, &m_vao);
        delete m_voxels;
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

        v1.normal = v2.normal = v3.normal = v4.normal = glm::vec3(0, -1, 0);
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

        v1.normal = v2.normal = v3.normal = v4.normal = glm::vec3(0, 1, 0);
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

        v1.normal = v2.normal = v3.normal = v4.normal = glm::vec3(-1, 0, 0);
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

        v1.normal = v2.normal = v3.normal = v4.normal = glm::vec3(1, 0, 0);
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

        v1.normal = v2.normal = v3.normal = v4.normal = glm::vec3(0, 0, 1);
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

        v1.normal = v2.normal = v3.normal = v4.normal = glm::vec3(0, 0, -1);
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
        using Clock = std::chrono::high_resolution_clock;
        auto startTime = Clock::now();

        std::vector<Vertex> vertices;
        vertices.reserve(m_numVertices);

        for (int y = width - 1; y >= 0; y--)
        {
            for (int z = 0; z < width; z++)
            {
                for (int x = 0; x < width; x++)
                {
                    if (GetVoxel(x, y, z) == Voxel::Air)
                        continue;

                    glm::vec3 center = {
                        x * 1.0f / 16.0f,
                        y * 1.0f / 16.0f,
                        z * 1.0f / 16.0f
                    };

                    glm::vec3 color = glm::vec3((float)rand() / (float)RAND_MAX, 1.0f, 0.5f);


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


        auto duration = Clock::now() - startTime;
        float time = std::chrono::duration_cast<std::chrono::microseconds>(duration).count() / 1000.0f;
        std::cout << "chunk mesh generated (" << time << "ms)\n";
    }
}

