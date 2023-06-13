#include <iostream>
#include "VoxelRenderer.h"

int main()
{
	std::cout << "pozdravljen svet\n";

	voxr::CreateWindow("VoxelsTest", 1920, 1080);
	
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

		for (float x = -2.5f; x <= 2.5f; x += 1.0f / 16.0f)
			for (float z = -2.5f; z <= 2.5f; z += 1.0f / 16.0f)
				voxr::DrawCube(glm::vec3(x, 0, z), glm::vec3(0.098f, 0.66f, 0.25f));

		voxr::DrawTextF("%.0ffps", glm::vec2(0.0f, 0.0f), 1.0f / deltaTime);
		voxr::DrawTextF("%.3fms", glm::vec2(0.0f, 30.0f), deltaTime * 1000.0f);

		glfwSwapBuffers(voxr::GetWindow());
	}

	glfwTerminate();
}