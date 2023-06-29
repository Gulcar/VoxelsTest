#pragma once

#include <cstring>
#include <stdint.h>
#include <stdlib.h>
#include <glm/vec3.hpp>
#include <assert.h>

namespace voxr
{

enum class Voxel : uint8_t
{
    Air = 0,
    Grass,
    Sand,
    Water,
    Wood,
    Leaf
};

class Chunk
{
public:
    Chunk();
    ~Chunk();

    inline Voxel GetVoxel(int x, int y, int z) const
    {
        AssertIndex(x, y, z);
        return m_voxels[x + y * width + z * width * width];
    }

    inline void SetVoxel(Voxel v, int x, int y, int z)
    {
        AssertIndex(x, y, z);
        m_voxels[x + y * width + z * width * width] = v;
    }

    inline void Clear()
    {
        memset(m_voxels, 0, width * width * width);
    }

    inline uint32_t GetVao() const { return m_vao; }
    inline size_t GetNumVertices() const { return m_numVertices; }
    inline voxr::Voxel* GetData() { return m_voxels; }

    void GenerateMesh();

    static constexpr int width = 64;
    static constexpr float worldWidth = width * 1.0f / 16.0f;
    
private:
    Voxel* m_voxels;
    uint32_t m_vao, m_vbo;
    size_t m_numVertices = 0;


private:
    void AssertIndex(int x, int y, int z) const
    {
        assert(x >= 0 && x < width && "voxel index out of bounds!");
        assert(y >= 0 && y < width && "voxel index out of bounds!");
        assert(z >= 0 && z < width && "voxel index out of bounds!");
    }
};

}
