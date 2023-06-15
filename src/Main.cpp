#include <iostream>
#include "VoxelRenderer.h"
#include <noise/noise.h>

int main()
{
	std::cout << "pozdravljen svet\n";

	voxr::CreateWindow("VoxelsTest", 1920, 1080);

    voxr::Chunk chunk;
    chunk.Clear();

    noise::module::Perlin perlin;
    float perlinScale = 15.0f;

	for (int z = 0; z < chunk.width; z++)
        for (int y = 0; y < chunk.width; y++)
			for (int x = 0; x < chunk.width; x++)
                if (perlin.GetValue((float)x / perlinScale, (float)y / perlinScale, (float)z / perlinScale) > 0.3)
                    chunk.SetVoxel(voxr::Voxel::Grass, x, y, z);

    chunk.GenerateMesh();

	
	while (!glfwWindowShouldClose(voxr::GetWindow()))
	{
		static float prevTime = glfwGetTime();
		float time = glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		glfwPollEvents();

		voxr::UpdateCamera(deltaTime);

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
