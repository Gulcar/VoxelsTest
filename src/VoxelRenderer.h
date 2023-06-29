#pragma once

#include <string_view>
#include <cstdint>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#undef CreateWindow
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "Chunk.h"
#include "FrustumCulling.h"

constexpr auto PI = 3.14159265359f;

#define USE_DEBUG_CAMERA 0
#define RANDOM_SEED 1

namespace voxr
{
    void CreateWindow(const char* title, int width, int height);
    GLFWwindow* GetWindow();

    void UpdateCamera(float deltaTime);
    const glm::vec3& GetCameraForward();

    const glm::vec3& GetCameraPos();
    void SetCameraPos(const glm::vec3& pos);
    const glm::vec2& GetCameraRot();
    void SetCameraRot(const glm::vec2& rot);

    void DrawCube(const glm::vec3& pos, const glm::vec3 color = glm::vec3(1.0f));
    void DrawText(std::string_view text, glm::vec2 pos = glm::vec2(0.0f));
    void DrawTextF(std::string_view format, glm::vec2 pos = glm::vec2(0.0f), ...);
    void DrawChunk(const Chunk& chunk, const glm::vec3& pos);

    void DrawLine(const glm::vec3& a, const glm::vec3& b);
    void SubmitDrawLines();

    void ShadowPass();

    uint32_t LoadShader(std::string_view source, GLenum type);
    uint32_t LoadShaderProgram(const char* vertShaderFile, const char* fragShaderFile);
}
