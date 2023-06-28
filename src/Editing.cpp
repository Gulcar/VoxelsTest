#include "Editing.h"
#include "VoxelRenderer.h"
#include "ChunkManager.h"

namespace voxr
{

void HandleVoxelEditing(voxr::Physics::HitResult& hit, float deltaTime)
{
    static float timeHoldingRight = 0.0f;
    static float timeHoldingLeft = 0.0f;

    constexpr float editsPerSecond = 17.5f;

    if (glfwGetMouseButton(voxr::GetWindow(), GLFW_MOUSE_BUTTON_RIGHT))
    {
        if (timeHoldingRight == 0.0f || timeHoldingRight > 0.5f)
        {
            hit.chunk->SetVoxel(voxr::Voxel::Air, hit.voxelIndex.x, hit.voxelIndex.y, hit.voxelIndex.z);

            if (hit.voxelIndex.x != 0)
                hit.chunk->SetVoxel(voxr::Voxel::Air, hit.voxelIndex.x - 1, hit.voxelIndex.y, hit.voxelIndex.z);
            else
                voxr::ChunkManager::GetChunk(hit.chunkIndex.x - 1, hit.chunkIndex.y)->SetVoxel(voxr::Voxel::Air, voxr::Chunk::width - 1, hit.voxelIndex.y, hit.voxelIndex.z);

            if (hit.voxelIndex.y != 0)
                hit.chunk->SetVoxel(voxr::Voxel::Air, hit.voxelIndex.x, hit.voxelIndex.y - 1, hit.voxelIndex.z);

            if (hit.voxelIndex.z != 0)
                hit.chunk->SetVoxel(voxr::Voxel::Air, hit.voxelIndex.x, hit.voxelIndex.y, hit.voxelIndex.z - 1);
            else
                voxr::ChunkManager::GetChunk(hit.chunkIndex.x, hit.chunkIndex.y - 1)->SetVoxel(voxr::Voxel::Air, hit.voxelIndex.x, hit.voxelIndex.y, voxr::Chunk::width - 1);

            //

            if (hit.voxelIndex.x != voxr::Chunk::width - 1)
                hit.chunk->SetVoxel(voxr::Voxel::Air, hit.voxelIndex.x + 1, hit.voxelIndex.y, hit.voxelIndex.z);
            else
                voxr::ChunkManager::GetChunk(hit.chunkIndex.x + 1, hit.chunkIndex.y)->SetVoxel(voxr::Voxel::Air, 0, hit.voxelIndex.y, hit.voxelIndex.z);

            if (hit.voxelIndex.y != voxr::Chunk::width - 1)
                hit.chunk->SetVoxel(voxr::Voxel::Air, hit.voxelIndex.x, hit.voxelIndex.y + 1, hit.voxelIndex.z);

            if (hit.voxelIndex.z != voxr::Chunk::width - 1)
                hit.chunk->SetVoxel(voxr::Voxel::Air, hit.voxelIndex.x, hit.voxelIndex.y, hit.voxelIndex.z + 1);
            else
                voxr::ChunkManager::GetChunk(hit.chunkIndex.x, hit.chunkIndex.y + 1)->SetVoxel(voxr::Voxel::Air, hit.voxelIndex.x, hit.voxelIndex.y, 0);


            hit.chunk->GenerateMesh();
            timeHoldingRight -= 1.0f / editsPerSecond;
        }

        timeHoldingRight += deltaTime;
    }
    else
    {
        timeHoldingRight = 0.0f;
    }

    if (glfwGetMouseButton(voxr::GetWindow(), GLFW_MOUSE_BUTTON_LEFT))
    {
        if (timeHoldingLeft == 0.0f || timeHoldingLeft > 0.5f)
        {
            if (hit.voxelIndex.x != 0)
                hit.chunk->SetVoxel(hit.voxel, hit.voxelIndex.x - 1, hit.voxelIndex.y, hit.voxelIndex.z);
            else
                voxr::ChunkManager::GetChunk(hit.chunkIndex.x - 1, hit.chunkIndex.y)->SetVoxel(hit.voxel, voxr::Chunk::width - 1, hit.voxelIndex.y, hit.voxelIndex.z);

            if (hit.voxelIndex.y != 0)
                hit.chunk->SetVoxel(hit.voxel, hit.voxelIndex.x, hit.voxelIndex.y - 1, hit.voxelIndex.z);

            if (hit.voxelIndex.z != 0)
                hit.chunk->SetVoxel(hit.voxel, hit.voxelIndex.x, hit.voxelIndex.y, hit.voxelIndex.z - 1);
            else
                voxr::ChunkManager::GetChunk(hit.chunkIndex.x, hit.chunkIndex.y - 1)->SetVoxel(hit.voxel, hit.voxelIndex.x, hit.voxelIndex.y, voxr::Chunk::width - 1);

            //

            if (hit.voxelIndex.x != voxr::Chunk::width - 1)
                hit.chunk->SetVoxel(hit.voxel, hit.voxelIndex.x + 1, hit.voxelIndex.y, hit.voxelIndex.z);
            else
                voxr::ChunkManager::GetChunk(hit.chunkIndex.x + 1, hit.chunkIndex.y)->SetVoxel(hit.voxel, 0, hit.voxelIndex.y, hit.voxelIndex.z);

            if (hit.voxelIndex.y != voxr::Chunk::width - 1)
                hit.chunk->SetVoxel(hit.voxel, hit.voxelIndex.x, hit.voxelIndex.y + 1, hit.voxelIndex.z);

            if (hit.voxelIndex.z != voxr::Chunk::width - 1)
                hit.chunk->SetVoxel(hit.voxel, hit.voxelIndex.x, hit.voxelIndex.y, hit.voxelIndex.z + 1);
            else
                voxr::ChunkManager::GetChunk(hit.chunkIndex.x, hit.chunkIndex.y + 1)->SetVoxel(hit.voxel, hit.voxelIndex.x, hit.voxelIndex.y, 0);


            hit.chunk->GenerateMesh();
            timeHoldingLeft -= 1.0f / editsPerSecond;
        }

        timeHoldingLeft += deltaTime;
    }
    else
    {
        timeHoldingLeft = 0.0f;
    }
}

}
