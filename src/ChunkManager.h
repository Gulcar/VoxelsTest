#pragma once

#include "Chunk.h"
#include <glm/vec3.hpp>

namespace voxr
{

    namespace ChunkManager
    {
        void GenerateChunks();
        void DeleteChunks();

        void UpdateCameraPos(const glm::vec3& camPos);
        void RenderChunks();

        void FlushLoadQueue();

        int GetSeed();
        void SetSeed(int seed);

        Chunk* GetChunk(int x, int z);
        void SetChunk(Chunk* chunk, int x, int z);

        const glm::vec3& GetCenterChunkPos();
        void SetCenterChunkPos(const glm::vec3& pos);

        inline constexpr int width = 11;
    }

}
