#pragma once

#include "Chunk.h"
#include <glm/vec3.hpp>
#include <array>

namespace voxr
{
    bool IsChunkInView(voxr::Chunk* chunk, const glm::vec3& pos);

    void UpdateCameraFrustum(const glm::vec3& camPos, const glm::vec3& camForward,
        const glm::vec3& camRight, float zNear, float zFar, float aspect);

    const std::array<glm::vec3, 8>& GetCamFrustumCorners();
}
