#include "Chunk.h"
#include "VoxelRenderer.h"
#include <vector>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
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
    }

    Chunk::~Chunk()
    {
        //glDeleteVertexArrays(1, &m_vao);
    }

    void AddFaceBottom(const glm::vec3& center, std::vector<Vertex>& vertices)
    {
        Vertex v1;
        v1.pos = center + glm::vec3(-r, -r, -r);
        Vertex v2;
        v2.pos = center + glm::vec3(r, -r, -r);
        Vertex v3;
        v3.pos = center + glm::vec3(r, -r, r);
        Vertex v4;
        v4.pos = center + glm::vec3(-r, -r, r);

        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);

        vertices.push_back(v3);
        vertices.push_back(v4);
        vertices.push_back(v1);
    }
    
    void AddFaceTop(const glm::vec3& center, std::vector<Vertex>& vertices)
    {
        Vertex v1;
        v1.pos = center + glm::vec3(-r, r, -r);
        Vertex v2;
        v2.pos = center + glm::vec3(r, r, -r);
        Vertex v3;
        v3.pos = center + glm::vec3(r, r, r);
        Vertex v4;
        v4.pos = center + glm::vec3(-r, r, r);

        vertices.push_back(v3);
        vertices.push_back(v2);
        vertices.push_back(v1);

        vertices.push_back(v1);
        vertices.push_back(v4);
        vertices.push_back(v3);
    }
    
    void AddFaceLeft(const glm::vec3& center, std::vector<Vertex>& vertices)
    {
        Vertex v1;
        v1.pos = center + glm::vec3(-r, -r, -r);
        Vertex v2;
        v2.pos = center + glm::vec3(-r, -r, r);
        Vertex v3;
        v3.pos = center + glm::vec3(-r, r, r);
        Vertex v4;
        v4.pos = center + glm::vec3(-r, r, -r);

        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);

        vertices.push_back(v3);
        vertices.push_back(v4);
        vertices.push_back(v1);
    }

    void AddFaceRight(const glm::vec3& center, std::vector<Vertex>& vertices)
    {
        Vertex v1;
        v1.pos = center + glm::vec3(r, -r, -r);
        Vertex v2;
        v2.pos = center + glm::vec3(r, -r, r);
        Vertex v3;
        v3.pos = center + glm::vec3(r, r, r);
        Vertex v4;
        v4.pos = center + glm::vec3(r, r, -r);

        vertices.push_back(v3);
        vertices.push_back(v2);
        vertices.push_back(v1);

        vertices.push_back(v1);
        vertices.push_back(v4);
        vertices.push_back(v3);
    }

    void AddFaceFront(const glm::vec3& center, std::vector<Vertex>& vertices)
    {
        Vertex v1;
        v1.pos = center + glm::vec3(-r, -r, r);
        Vertex v2;
        v2.pos = center + glm::vec3(r, -r, r);
        Vertex v3;
        v3.pos = center + glm::vec3(r, r, r);
        Vertex v4;
        v4.pos = center + glm::vec3(-r, r, r);

        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);

        vertices.push_back(v3);
        vertices.push_back(v4);
        vertices.push_back(v1);
    }

    void AddFaceBack(const glm::vec3& center, std::vector<Vertex>& vertices)
    {
        Vertex v1;
        v1.pos = center + glm::vec3(-r, -r, -r);
        Vertex v2;
        v2.pos = center + glm::vec3(r, -r, -r);
        Vertex v3;
        v3.pos = center + glm::vec3(r, r, -r);
        Vertex v4;
        v4.pos = center + glm::vec3(-r, r, -r);

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
        vertices.reserve(m_numVertices);

        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < width; y++)
            {
                for (int z = 0; z < width; z++)
                {
                    if (GetVoxel(x, y, z) == Voxel::Air)
                        continue;

                    glm::vec3 center = {
                        x * 1.0f / 16.0f,
                        y * 1.0f / 16.0f,
                        z * 1.0f / 16.0f
                    };

                    AddFaceBottom(center, vertices);
                    AddFaceTop(center, vertices);
                    AddFaceLeft(center, vertices);
                    AddFaceRight(center, vertices);
                    AddFaceFront(center, vertices);
                    AddFaceBack(center, vertices);
                }
            }
        }

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
        
        m_numVertices = vertices.size();
    }
}

