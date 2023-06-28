#include "Save.h"
#include <portable-file-dialogs/portable-file-dialogs.h>
#include "Chunk.h"
#include "ChunkManager.h"
#include "VoxelRenderer.h"
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <iostream>
#include <string>
#include <fstream>

namespace voxr::Save
{
    struct SaveData
    {
        glm::vec3 camPos;
        glm::vec2 camRot;
        glm::vec3 centerChunkPos;
        voxr::Voxel voxelData[ChunkManager::width][ChunkManager::width][Chunk::width * Chunk::width * Chunk::width];
    };

    void AddFileExtension(std::string& s)
    {
        constexpr std::string_view ext = ".vxl";
        int len = s.length();

        if (len < 5 || s.compare(len - ext.length(), ext.length(), ext) != 0)
        {
            s += ext;
        }
    }

    void OpenWorld()
    {
        pfd::open_file dialog("Open Your Voxel World", ".", { "Voxel World File ", "*.vxl", "All Files", "*" }, pfd::opt::none);
        auto dialogResult = dialog.result();
        if (dialogResult.empty()) return;
        std::string fileName = dialogResult[0];
        if (fileName.empty()) return;
        AddFileExtension(fileName);

        voxr::ChunkManager::FlushLoadQueue();

        std::ifstream file(fileName, std::ios::binary);
        if (file.is_open())
        {
            SaveData* data = new SaveData;
            file.read((char*)data, sizeof(SaveData));

            voxr::SetCameraPos(data->camPos);
            voxr::SetCameraRot(data->camRot);
            ChunkManager::SetCenterChunkPos(data->centerChunkPos);

            ChunkManager::DeleteChunks();

            for (int z = 0; z < ChunkManager::width; z++)
            {
                for (int x = 0; x < ChunkManager::width; x++)
                {
                    Chunk* chunk = new Chunk;
                    ChunkManager::SetChunk(chunk, x, z);

                    memcpy(chunk->GetData(), data->voxelData[z][x], Chunk::width * Chunk::width * Chunk::width);

                    chunk->GenerateMesh();
                }
            }

            delete data;
            std::cout << "opened world " << fileName << "\n";
        }
        else
        {
            std::cout << "failed to open world " << fileName << "\n";
        }
    }

    void SaveWorld()
    {
        pfd::save_file dialog("Save Your Voxel World", ".", { "Voxel World File ", "*.vxl", "All Files", "*" }, pfd::opt::none);
        std::string fileName = dialog.result();
        if (fileName.empty()) return;
        AddFileExtension(fileName);

        voxr::ChunkManager::FlushLoadQueue();

        SaveData* data = new SaveData;
        data->camPos = voxr::GetCameraPos();
        data->camRot = voxr::GetCameraRot();
        data->centerChunkPos = voxr::ChunkManager::GetCenterChunkPos();
        
        for (int z = 0; z < ChunkManager::width; z++)
        {
            for (int x = 0; x < ChunkManager::width; x++)
            {
                memcpy(data->voxelData[z][x], ChunkManager::GetChunk(x, z)->GetData(), Chunk::width * Chunk::width * Chunk::width);
            }
        }

        std::ofstream file(fileName, std::ios::binary);
        
        if (file.is_open())
        {
            file.write((const char*)data, sizeof(SaveData));
            std::cout << "saved world to " << fileName << "\n";
        }
        else
        {
            std::cout << "failed to save world to " << fileName << "\n";
        }

        delete data;
    }
}
