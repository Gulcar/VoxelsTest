#include "ChunkManager.h"
#include "VoxelRenderer.h"
#include "FuncTimer.h"
#include <glm/glm.hpp>
#include <noise/noise.h>
#include <assert.h>
#include <chrono>
#include <iostream>
#include <deque>

namespace
{
    voxr::Chunk* m_chunks[voxr::ChunkManager::width][voxr::ChunkManager::width];
    glm::vec3 m_centerChunkPos;

    struct LoadItem
    {
        voxr::Chunk* chunk;
        glm::vec2 pos;
    };
    std::deque<LoadItem> m_loadQueue;

    noise::module::Perlin m_perlin;


    void PerlinTerrain(voxr::Chunk* chunk, glm::vec2 offset)
    {
        chunk->Clear();

        constexpr float perlinScale = 5.0f;

        for (int z = 0; z < chunk->width; z++)
        {
            for (int x = 0; x < chunk->width; x++)
            {
                float fx = ((float)x * 1.0f / 16.0f + offset.x) / perlinScale;
                float fz = ((float)z * 1.0f / 16.0f + offset.y) / perlinScale;

                float height = m_perlin.GetValue(fx, 0.0f, fz);
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

                if (blocks > 19 && blocks < 25 && x % 2 != z % 2
                    && m_perlin.GetValue(fx, 69.0f, fz) > 0.3f && m_perlin.GetValue(fx * 1000.0f, 69.0f, fz * 1000.0f) > 0.7f
                    && x > 1 && z > 1 && x < voxr::Chunk::width - 2 && z < voxr::Chunk::width - 2)
                {
                    for (int y = blocks; y < blocks + 5; y++)
                        chunk->SetVoxel(voxr::Voxel::Wood, x, y, z);

                    chunk->SetVoxel(voxr::Voxel::Leaf, x, blocks + 5, z);

                    chunk->SetVoxel(voxr::Voxel::Leaf, x-1, blocks + 4, z);
                    chunk->SetVoxel(voxr::Voxel::Leaf, x, blocks + 4, z-1);
                    chunk->SetVoxel(voxr::Voxel::Leaf, x+1, blocks + 4, z);
                    chunk->SetVoxel(voxr::Voxel::Leaf, x, blocks + 4, z+1);

                    chunk->SetVoxel(voxr::Voxel::Leaf, x - 2, blocks + 3, z);
                    chunk->SetVoxel(voxr::Voxel::Leaf, x, blocks + 3, z - 2);
                    chunk->SetVoxel(voxr::Voxel::Leaf, x + 2, blocks + 3, z);
                    chunk->SetVoxel(voxr::Voxel::Leaf, x, blocks + 3, z + 2);

                    if (m_perlin.GetValue(fx * 1000.0f, -69.0f, fz * 1000.0f) > 0.2f)
                        chunk->SetVoxel(voxr::Voxel::Leaf, x - 1, blocks + 3, z - 1);
                    if (m_perlin.GetValue(fx * 1000.0f, 169.0f, fz * 1000.0f) > 0.2f)
                        chunk->SetVoxel(voxr::Voxel::Leaf, x - 1, blocks + 3, z + 1);
                    if (m_perlin.GetValue(fx * 1000.0f, 269.0f, fz * 1000.0f) > 0.2f)
                        chunk->SetVoxel(voxr::Voxel::Leaf, x + 1, blocks + 3, z - 1);
                    if (m_perlin.GetValue(fx * 1000.0f, 369.0f, fz * 1000.0f) > 0.2f)
                        chunk->SetVoxel(voxr::Voxel::Leaf, x + 1, blocks + 3, z + 1);
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

#if RANDOM_SEED
            m_perlin.SetSeed(time(nullptr));
#endif

            for (int z = width - 1; z >= 0; z--)
            {
                for (int x = 0; x < width; x++)
                {
                    Chunk* chunk = new Chunk;
                    SetChunk(chunk, x, z);
                    //PerlinTerrain(chunk, glm::vec2(Chunk::worldWidth * x, Chunk::worldWidth * z));
                    m_loadQueue.push_back({ chunk, glm::vec2(Chunk::worldWidth * x, Chunk::worldWidth * z) });
                }
            }
        }

        void DeleteChunks()
        {
            for (int z = 0; z < width; z++)
                for (int x = 0; x < width; x++)
                    delete m_chunks[z][x];
        }

        void DeleteChunk(int x, int z)
        {
            Chunk* chunk = GetChunk(x, z);

            // ce ne izbrisem se iz load queue-ja potem bo crash ko bo prisel na vrsto za loadanje
            for (auto it = m_loadQueue.begin(); it != m_loadQueue.end(); it++)
            {
                if (it->chunk == chunk)
                {
                    m_loadQueue.erase(it);
                    break;
                }
            }

            delete chunk;
        }

        void UpdateCameraRight()
        {
            m_centerChunkPos.x += Chunk::worldWidth;

            for (int z = 0; z < width; z++)
                DeleteChunk(0, z);

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

                m_loadQueue.push_back({ chunk, pos });
                //PerlinTerrain(chunk, pos);
            }
        }

        void UpdateCameraLeft()
        {
            m_centerChunkPos.x -= Chunk::worldWidth;

            for (int z = 0; z < width; z++)
                DeleteChunk(width - 1, z);

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

                m_loadQueue.push_back({ chunk, pos });
                //PerlinTerrain(chunk, pos);
            }
        }

        void UpdateCameraForward()
        {
            m_centerChunkPos.z += Chunk::worldWidth;

            for (int x = 0; x < width; x++)
                DeleteChunk(x, 0);

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

                m_loadQueue.push_back({ chunk, pos });
                //PerlinTerrain(chunk, pos);
            }
        }

        void UpdateCameraBackward()
        {
            m_centerChunkPos.z -= Chunk::worldWidth;

            for (int x = 0; x < width; x++)
                DeleteChunk(x, width - 1);

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

                m_loadQueue.push_back({ chunk, pos });
                //PerlinTerrain(chunk, pos);
            }
        }

        void UpdateCameraPos(const glm::vec3& camPos)
        {
            constexpr float chunkUpdateWidth = Chunk::worldWidth / 1.7f;

            if (camPos.x > m_centerChunkPos.x + chunkUpdateWidth)
            {
                UpdateCameraRight();
            }
            else if (camPos.x < m_centerChunkPos.x - chunkUpdateWidth)
            {
                UpdateCameraLeft();
            }
            else if (camPos.z > m_centerChunkPos.z + chunkUpdateWidth)
            {
                UpdateCameraForward();
            }
            else if (camPos.z < m_centerChunkPos.z - chunkUpdateWidth)
            {
                UpdateCameraBackward();
            }

            //while (m_loadQueue.empty() == false)
            if (m_loadQueue.empty() == false)
            {
                const LoadItem& loadItem = m_loadQueue.front();

                PerlinTerrain(loadItem.chunk, loadItem.pos);

                m_loadQueue.pop_front();
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
                    pos += glm::vec3(1.0f / 16.0f / 2.0f);
                    pos += m_centerChunkPos;

                    if (voxr::IsChunkInView(chunk, pos))
                        voxr::DrawChunk(*chunk, pos);
                }
            }
        }

        void FlushLoadQueue()
        {
            while (m_loadQueue.empty() == false)
            {
                const LoadItem& loadItem = m_loadQueue.front();
                PerlinTerrain(loadItem.chunk, loadItem.pos);
                m_loadQueue.pop_front();
            }
        }

        int GetSeed()
        {
            return m_perlin.GetSeed();
        }

        void SetSeed(int seed)
        {
            m_perlin.SetSeed(seed);
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

        const glm::vec3& GetCenterChunkPos()
        {
            return m_centerChunkPos;
        }

        void SetCenterChunkPos(const glm::vec3& pos)
        {
            m_centerChunkPos = pos;
        }
    }

}
