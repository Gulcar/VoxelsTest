#include <iostream>
#include "VoxelRenderer.h"
#include <noise/noise.h>
#include <omp.h>

void PerlinTerrain(voxr::Chunk& chunk, glm::vec2 offset)
{
    chunk.Clear();

    noise::module::Perlin perlin;
    constexpr float perlinScale = 100.0f;

    for (int z = 0; z < chunk.width; z++)
    {
        for (int x = 0; x < chunk.width; x++)
        {
            float height = perlin.GetValue((float)x / perlinScale + offset.x, 0.0f, (float)z / perlinScale + offset.y);
            height = (height + 2.0f) / 8.0f;

            int blocks = height * chunk.width;

            for (int y = 0; y < blocks; y++)
            {
                voxr::Voxel v;
                if (y > 48 && y < 52) v = voxr::Voxel::Sand;
                else if (y > 46 && y < 54 && rand() % 2 == 0) v = voxr::Voxel::Sand;
                else v = voxr::Voxel::Grass;
                chunk.SetVoxel(v, x, y, z);
            }

            constexpr int waterHeight = 50;
            for (int y = blocks; y < waterHeight; y++)
            {
                chunk.SetVoxel(voxr::Voxel::Water, x, y, z);
            }
        }
    }

    chunk.GenerateMesh();
}

int main()
{
    std::cout << "pozdravljen svet\n";

    voxr::CreateWindow("VoxelsTest", 1920, 1080);

    voxr::Chunk chunk;
    float xoffset = 0.0f;

    PerlinTerrain(chunk, glm::vec2(xoffset, 0));

    while (!glfwWindowShouldClose(voxr::GetWindow()))
    {
        static float prevTime = glfwGetTime();
        float time = glfwGetTime();
        float deltaTime = time - prevTime;
        prevTime = time;

        glfwPollEvents();

        voxr::UpdateCamera(deltaTime);

        if (glfwGetKey(voxr::GetWindow(), GLFW_KEY_SPACE))
        {
            xoffset += 0.1f;
            PerlinTerrain(chunk, glm::vec2(xoffset, 0));
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        voxr::DrawCube(glm::vec3(0, 0, 0), glm::vec3(1.0f, 0.1f, 0.1f));
        voxr::DrawCube(glm::vec3(0.1, 0, 0), glm::vec3(1.0f, 0.1f, 0.1f));
        voxr::DrawCube(glm::vec3(0.2, 0, 0), glm::vec3(1.0f, 0.1f, 0.1f));

        voxr::DrawChunk(chunk, glm::vec3(0, 0, 0));

        voxr::DrawTextF("%.0ffps", glm::vec2(0.0f, 0.0f), 1.0f / deltaTime);
        voxr::DrawTextF("%.3fms", glm::vec2(0.0f, 30.0f), deltaTime * 1000.0f);

        glfwSwapBuffers(voxr::GetWindow());
    }

    glfwTerminate();
}
