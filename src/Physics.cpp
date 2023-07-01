#include "Physics.h"
#include "ChunkManager.h"
#include "VoxelRenderer.h"
#include <glm/glm.hpp>

namespace voxr::Physics
{
    namespace
    {
        constexpr float m_gravity = -9.81f;
        bool m_useGravity = false;
        float m_velocity = 0.0f;
    }

    bool RaycastThroughChunk(const Ray& ray, voxr::Chunk* chunk, const glm::vec3& chunkPos, HitResult* outHit, float tmax, float* tout)
    {
        bool didHit = false;

        for (int y = Chunk::width - 1; y >= 0; y--)
        {
            for (int z = 0; z < Chunk::width; z++)
            {
                for (int x = 0; x < Chunk::width; x++)
                {
                    voxr::Voxel voxel = chunk->GetVoxel(x, y, z);
                    if (voxel == voxr::Voxel::Air)
                        continue;

                    constexpr glm::vec3 aabbSize = glm::vec3(1.0f / 16.0f);

                    glm::vec3 pos = {
                        (x - Chunk::width / 2.0f) * 1.0f / 16.0f,
                        (y - Chunk::width / 2.0f) * 1.0f / 16.0f,
                        (z - Chunk::width / 2.0f) * 1.0f / 16.0f
                    };
                    pos += chunkPos;

                    AABB aabb;
                    aabb.min = pos - aabbSize / 2.0f * 1.05f;
                    aabb.max = pos + aabbSize / 2.0f * 1.05f;

                    float t;
                    if (RayAABBIntersection(ray, aabb, tmax, &t))
                    {
                        didHit = true;
                        tmax = t;
                        *tout = t;

                        outHit->pos = pos;
                        outHit->voxel = voxel;
                        outHit->voxelIndex = glm::ivec3(x, y, z);
                    }
                }
            }
        }

        return didHit;
    }

    bool Raycast(const Ray& ray, HitResult* outHit, float tmax)
    {
        glm::vec3 centerChunkPos = ChunkManager::GetCenterChunkPos();
        bool didHit = false;

        for (int z = 0; z < ChunkManager::width; z++)
        {
            for (int x = 0; x < ChunkManager::width; x++)
            {
                constexpr glm::vec3 aabbSize = glm::vec3(Chunk::worldWidth);

                glm::vec3 pos = glm::vec3(Chunk::worldWidth * x, 0.0f, Chunk::worldWidth * z);
                pos.x -= ChunkManager::width / 2 * Chunk::worldWidth;
                pos.z -= ChunkManager::width / 2 * Chunk::worldWidth;
                pos += glm::vec3(1.0f / 16.0f / 2.0f);
                pos += centerChunkPos;

                AABB aabb;
                aabb.min = pos - aabbSize / 2.0f * 1.05f;
                aabb.max = pos + aabbSize / 2.0f * 1.05f;

                float t;
                if (RayAABBIntersection(ray, aabb, tmax, &t))
                {
                    voxr::Chunk* chunk = ChunkManager::GetChunk(x, z);

                    if (RaycastThroughChunk(ray, chunk, pos, outHit, tmax, &t))
                    {
                        didHit = true;
                        tmax = t;
                        outHit->chunk = chunk;
                        outHit->chunkIndex = glm::ivec2(x, z);
                    }
                }
            }
        }

        return didHit;
    }

    // https://medium.com/@bromanz/another-view-on-the-classic-ray-aabb-intersection-algorithm-for-bvh-traversal-41125138b525
    bool RayAABBIntersection(const Ray& ray, const AABB& aabb, float tmax, float* t)
    {
        glm::vec3 invD = 1.0f / ray.dir;
        glm::vec3 t0s = (aabb.min - ray.origin) * invD;
        glm::vec3 t1s = (aabb.max - ray.origin) * invD;

        glm::vec3 tsmaller = glm::min(t0s, t1s);
        glm::vec3 tbigger = glm::max(t0s, t1s);

        using glm::max;
        using glm::min;

        float tmin = 0.01f;

        tmin = max(tmin, max(tsmaller[0], max(tsmaller[1], tsmaller[2])));
        tmax = min(tmax, min(tbigger[0], min(tbigger[1], tbigger[2])));

        if (tmin < tmax)
        {
            *t = tmin;
            return true;
        }

        return false;
    }

    void ApplyGravityToCamera(float deltaTime)
    {
        if (!m_useGravity) return;

        m_velocity += m_gravity * deltaTime;
        
        glm::vec3 camPos = voxr::GetCameraPos();
        camPos.y += m_velocity * deltaTime;

        constexpr float camHeight = 0.05f;

        Ray ray;
        ray.origin = camPos + glm::vec3(0, -camHeight, 0);
        ray.dir = glm::vec3(0.0f, -1.0f, 0.0f);

        HitResult hit;
        if (Raycast(ray, &hit, 0.5f))
        {
            float h = camPos.y - (hit.pos.y + 2.0f / 16.0f);
            if (h < camHeight)
            {
                float target = hit.pos.y + 2.0f / 16.0f + camHeight;
                camPos.y = glm::mix(camPos.y, target, 50.0f * deltaTime);
                m_velocity = 0.0f;
            }
        }

        voxr::SetCameraPos(camPos);
    }

    bool GetUseGravity()
    {
        return m_useGravity;
    }

    void SetUseGravity(bool gravity)
    {
        if (m_useGravity != gravity)
        {
            m_useGravity = gravity;
            m_velocity = 0.0f;
        }
    }
}
