#pragma once

#include <cstring>
#include <stdint.h>
#include <stdlib.h>
#include <glm/vec3.hpp>

namespace voxr
{

enum class Voxel : uint8_t
{
    Air = 0,
    Grass,
    Dirt,
    Stone,
    Wood
};

class Chunk
{
public:
    Chunk();
    ~Chunk();

    inline Voxel GetVoxel(int x, int y, int z) const
    {
        return m_voxels[z][y][x];
    }

    inline void SetVoxel(Voxel v, int x, int y, int z)
    {
        m_voxels[z][y][x] = v;
    }

    inline void Clear()
    {
        memset(m_voxels, 0, width * width * width);
    }

    inline uint32_t GetVao() const { return m_vao; }
    inline size_t GetNumVertices() const { return m_numVertices; }

    void GenerateMesh();

    static constexpr int width = 48;
    
private:
    Voxel m_voxels[width][width][width];
    uint32_t m_vao, m_vbo;
    size_t m_numVertices = 0;
};

}
