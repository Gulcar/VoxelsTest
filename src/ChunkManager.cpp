#include "ChunkManager.h"
#include <assert.h>
#include <glm/glm.hpp>
#include <noise/noise.h>
#include "VoxelRenderer.h"
#include <chrono>
#include <iostream>

namespace
{
    voxr::Chunk* m_chunks[voxr::ChunkManager::width][voxr::ChunkManager::width];

    glm::vec3 m_centerChunkPos;


    void PerlinTerrain(voxr::Chunk* chunk, glm::vec2 offset)
    {
        chunk->Clear();

        noise::module::Perlin perlin;
        constexpr float perlinScale = 5.0f;

        for (int z = 0; z < chunk->width; z++)
        {
            for (int x = 0; x < chunk->width; x++)
            {
                float fx = ((float)x * 1.0f / 16.0f + offset.x) / perlinScale;
                float fz = ((float)z * 1.0f / 16.0f + offset.y) / perlinScale;

                float height = perlin.GetValue(fx, 0.0f, fz);
                height = (height + 2.0f) / 6.0f;

                int blocks = height * chunk->width;

                for (int y = 0; y < blocks; y++)
                {
                    voxr::Voxel v;
                    if (y > 13 && y < 17) v = voxr::Voxel::Sand;
                    else if (y > 11 && y < 18 && rand() % 3 == 0) v = voxr::Voxel::Sand;
                    else v = voxr::Voxel::Grass;
                    chunk->SetVoxel(v, x, y, z);
                }

                constexpr int waterHeight = 15;
                for (int y = blocks; y < waterHeight; y++)
                {
                    chunk->SetVoxel(voxr::Voxel::Water, x, y, z);
                }
            }
        }

        chunk->GenerateMesh();
    }
}

namespace voxr
{

    namespace ChunkManager
    {
        void GenerateChunks()
        {
            m_centerChunkPos = glm::vec3(0, 0, 0);

            for (int z = 0; z < width; z++)
            {
                for (int x = 0; x < width; x++)
                {
                    Chunk* chunk = new Chunk;
                    SetChunk(chunk, x, z);
                    PerlinTerrain(chunk, glm::vec2(Chunk::worldWidth * x, Chunk::worldWidth * z));
                }
            }
        }

        void DeleteChunks()
        {
            for (int z = 0; z < width; z++)
                for (int x = 0; x < width; x++)
                    delete m_chunks[z][x];
        }

        void UpdateCameraPos(const glm::vec3& camPos)
        {
            constexpr float chunkUpdateWidth = Chunk::worldWidth / 1.7f;

            using Clock = std::chrono::high_resolution_clock;
            auto startTime = Clock::now();
            bool chunksUpdated = false;

            if (camPos.x > m_centerChunkPos.x + chunkUpdateWidth)
            {
                chunksUpdated = true;
                m_centerChunkPos.x += Chunk::worldWidth;
                
                for (int z = 0; z < width; z++)
                    delete GetChunk(0, z);

                for (int x = 1; x < width; x++)
                {
                    for (int z = 0; z < width; z++)
                    {
                        Chunk* chunk = GetChunk(x, z);
                        SetChunk(chunk, x - 1, z);
                    }
                }

                for (int z = 0; z < width; z++)
                {
                    Chunk* chunk = new Chunk;
                    SetChunk(chunk, width - 1, z);

                    glm::vec2 pos;
                    pos.x = m_centerChunkPos.x + Chunk::worldWidth * (width - 1);
                    pos.y = m_centerChunkPos.z + Chunk::worldWidth * (z);

                    PerlinTerrain(chunk, pos);
                }

            }
            else if (camPos.x < m_centerChunkPos.x - chunkUpdateWidth)
            {
                chunksUpdated = true;
                m_centerChunkPos.x -= Chunk::worldWidth;

                for (int z = 0; z < width; z++)
                    delete GetChunk(width - 1, z);

                for (int x = width - 2; x >= 0; x--)
                {
                    for (int z = 0; z < width; z++)
                    {
                        Chunk* chunk = GetChunk(x, z);
                        SetChunk(chunk, x + 1, z);
                    }
                }

                for (int z = 0; z < width; z++)
                {
                    Chunk* chunk = new Chunk;
                    SetChunk(chunk, 0, z);

                    glm::vec2 pos;
                    pos.x = m_centerChunkPos.x;
                    pos.y = m_centerChunkPos.z + Chunk::worldWidth * (z);

                    PerlinTerrain(chunk, pos);
                }
            }
            else if (camPos.z > m_centerChunkPos.z + chunkUpdateWidth)
            {
                chunksUpdated = true;
                m_centerChunkPos.z += Chunk::worldWidth;

                for (int x = 0; x < width; x++)
                    delete GetChunk(x, 0);

                for (int z = 1; z < width; z++)
                {
                    for (int x = 0; x < width; x++)
                    {
                        Chunk* chunk = GetChunk(x, z);
                        SetChunk(chunk, x, z - 1);
                    }
                }

                for (int x = 0; x < width; x++)
                {
                    Chunk* chunk = new Chunk;
                    SetChunk(chunk, x, width - 1);

                    glm::vec2 pos;
                    pos.x = m_centerChunkPos.x + Chunk::worldWidth * x;
                    pos.y = m_centerChunkPos.z + Chunk::worldWidth * (width - 1);

                    PerlinTerrain(chunk, pos);
                }
                
            }
            else if (camPos.z < m_centerChunkPos.z - chunkUpdateWidth)
            {
                chunksUpdated = true;
                m_centerChunkPos.z -= Chunk::worldWidth;
                
                for (int x = 0; x < width; x++)
                    delete GetChunk(x, width - 1);

                for (int z = width - 2; z >= 0; z--)
                {
                    for (int x = 0; x < width; x++)
                    {
                        Chunk* chunk = GetChunk(x, z);
                        SetChunk(chunk, x, z + 1);
                    }
                }

                for (int x = 0; x < width; x++)
                {
                    Chunk* chunk = new Chunk;
                    SetChunk(chunk, x, 0);

                    glm::vec2 pos;
                    pos.x = m_centerChunkPos.x + Chunk::worldWidth * x;
                    pos.y = m_centerChunkPos.z;

                    PerlinTerrain(chunk, pos);
                }
            }

            if (chunksUpdated)
            {
                float ftime = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - startTime).count() / 1000.0f;
                std::cout << "chunk manager updated chunks (" << ftime << "ms)\n";
            }
        }

        void RenderChunks()
        {
            for (int z = 0; z < width; z++)
            {
                for (int x = 0; x < width; x++)
                {
                    Chunk* chunk = GetChunk(x, z);
                    glm::vec3 pos = glm::vec3(Chunk::worldWidth * x, 0.0f, Chunk::worldWidth * z);
                    pos.x -= ChunkManager::width / 2 * Chunk::worldWidth;
                    pos.z -= ChunkManager::width / 2 * Chunk::worldWidth;
                    pos += m_centerChunkPos;

                    voxr::DrawChunk(*chunk, pos);
                }
            }
        }

        Chunk* GetChunk(int x, int z)
        {
            assert(x >= 0 && x < width && "chunk index out of bounds!");
            assert(z >= 0 && z < width && "chunk index out of bounds!");

            return m_chunks[z][x];
        }

        void SetChunk(Chunk* chunk, int x, int z)
        {
            assert(x >= 0 && x < width && "chunk index out of bounds!");
            assert(z >= 0 && z < width && "chunk index out of bounds!");

            m_chunks[z][x] = chunk;
        }
    }

}
