#include <iostream>
#include "VoxelRenderer.h"
#include "ChunkManager.h"
#include "Physics.h"
#include "Editing.h"

int main()
{
    std::cout << "pozdravljen svet\n";

    voxr::CreateWindow("VoxelsTest", 1920, 1080);

    voxr::ChunkManager::GenerateChunks();

    while (!glfwWindowShouldClose(voxr::GetWindow()))
    {
        static float prevTime = glfwGetTime();
        float time = glfwGetTime();
        float deltaTime = time - prevTime;
        prevTime = time;

        glfwPollEvents();

        voxr::Physics::ApplyGravityToCamera(deltaTime);

        voxr::UpdateCamera(deltaTime);
        voxr::ChunkManager::UpdateCameraPos(voxr::GetCameraPos());

        voxr::ShadowPass();

        glClearColor(0.471f, 0.831f, 0.941f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        voxr::DrawCube(glm::vec3(0, 0, 0), glm::vec3(1.0f, 0.1f, 0.1f));
        voxr::DrawCube(glm::vec3(0.1, 0, 0), glm::vec3(1.0f, 0.1f, 0.1f));
        voxr::DrawCube(glm::vec3(0.2, 0, 0), glm::vec3(1.0f, 0.1f, 0.1f));

        voxr::ChunkManager::RenderChunks();

        voxr::DrawTextF("%.0ffps", glm::vec2(0.0f, 0.0f), 1.0f / deltaTime);
        voxr::DrawTextF("%.3fms", glm::vec2(0.0f, 30.0f), deltaTime * 1000.0f);

        voxr::SubmitDrawLines();

        voxr::Physics::Ray ray;
        ray.origin = voxr::GetCameraPos();
        ray.dir = voxr::GetCameraForward();

        voxr::Physics::HitResult hit;
        if (voxr::Physics::Raycast(ray, &hit))
        {
            voxr::DrawCube(hit.pos - voxr::GetCameraForward() * 0.001f);
            voxr::HandleVoxelEditing(hit, deltaTime);
        }

        glfwSwapBuffers(voxr::GetWindow());
    }

    glfwTerminate();
}
