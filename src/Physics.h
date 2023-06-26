#pragma once

#include "Chunk.h"
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace voxr::Physics
{
    struct Ray
    {
        glm::vec3 origin;
        glm::vec3 dir;
    };

    struct AABB
    {
        glm::vec3 min;
        glm::vec3 max;
    };

    struct HitResult
    {
        glm::vec3 pos;
        voxr::Chunk* chunk;
        voxr::Voxel voxel;
        glm::ivec2 chunkIndex;
        glm::ivec3 voxelIndex;
    };

    bool Raycast(const Ray& ray, HitResult* outHit);

    bool RayAABBIntersection(const Ray& ray, const AABB& aabb, float tmax, float* t);
}
