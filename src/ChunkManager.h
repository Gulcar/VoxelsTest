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

        Chunk* GetChunk(int x, int z);
        void SetChunk(Chunk* chunk, int x, int z);

        const glm::vec3& GetCenterChunkPos();

        inline constexpr int width = 11;
    }

}
