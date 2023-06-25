#include "FrustumCulling.h"
#include "VoxelRenderer.h"
#include <glm/glm.hpp>
#include <vector>

namespace
{
    struct Plane
    {
        glm::vec3 normal;
        float dist; // dist je koliko normal da pridemo do ravnine!!
    };

    struct AABB
    {
        glm::vec3 center;
        glm::vec3 extents;
    };

    Plane m_nearPlane;
    Plane m_farPlane;
    Plane m_leftPlane;
    Plane m_rightPlane;
    Plane m_topPlane;
    Plane m_bottomPlane;

    uint32_t m_vao, m_vbo;
    glm::vec3 m_drawBuffer[35];

    // ali je aabb na pozitivni strani ravnine
    bool IsAABBInPlane(const AABB& aabb, const Plane& plane)
    {
        // https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling
        float r = aabb.extents.x * std::abs(plane.normal.x) +
            aabb.extents.y * std::abs(plane.normal.y) + aabb.extents.z * std::abs(plane.normal.z);

        float dist = glm::dot(plane.normal, aabb.center) - plane.dist;

        return -r <= dist;
    }
}

namespace voxr
{
    bool IsChunkInView(voxr::Chunk* chunk, const glm::vec3& pos)
    {
        AABB aabb;
        aabb.center = pos;
        aabb.extents = glm::vec3(Chunk::worldWidth / 2.0f);

        if (
            IsAABBInPlane(aabb, m_nearPlane) &&
            IsAABBInPlane(aabb, m_leftPlane) &&
            IsAABBInPlane(aabb, m_rightPlane) &&
            IsAABBInPlane(aabb, m_bottomPlane) &&
            IsAABBInPlane(aabb, m_topPlane) &&
            IsAABBInPlane(aabb, m_farPlane)
        )
            return true;

        return false;
    }

    void UpdateCameraFrustum(const glm::vec3& camPos, const glm::vec3& camForward,
        const glm::vec3& camRight, float zNear, float zFar, float aspect)
    {
        glm::vec3 camUp = glm::cross(camRight, camForward);

        // fakfchaslfha nigagaga  tukaj moras kot deliti z 2 !!!!!
        float nearHeight = glm::tan(glm::radians(60.0f) / 2.0f) * zNear * 2.0f;
        float nearWidth = nearHeight * aspect;

        float farHeight = glm::tan(glm::radians(60.0f) / 2.0f) * zFar * 2.0f;
        float farWidth = farHeight * aspect;

        glm::vec3 nearCenter = camPos + camForward * zNear;

        glm::vec3 nearBottomLeft = nearCenter - camRight * nearWidth / 2.0f - camUp * nearHeight / 2.0f;
        glm::vec3 nearTopLeft = nearBottomLeft + camUp * nearHeight;

        glm::vec3 nearBottomRight = nearBottomLeft + camRight * nearWidth;
        glm::vec3 nearTopRight = nearTopLeft + camRight * nearWidth;
        
        glm::vec3 farCenter = camPos + camForward * zFar;
        glm::vec3 farLeft = farCenter - camRight * farWidth / 2.0f;
        glm::vec3 farRight = farLeft + camRight * farWidth;

        glm::vec3 farTop = farCenter + camUp * farHeight / 2.0f;
        glm::vec3 farBottom = farTop - camUp * farHeight;

        m_nearPlane.normal = camForward;
        m_nearPlane.dist = glm::dot(nearCenter, camForward);

        m_farPlane.normal = -camForward;
        m_farPlane.dist = glm::dot(camPos + camForward * zFar, -camForward);

        m_leftPlane.normal = glm::normalize(glm::cross(farLeft - nearTopLeft, farLeft - nearBottomLeft));
        // dist je koliko normal da pridemo do ravnine!!
        m_leftPlane.dist = glm::dot(nearBottomLeft, m_leftPlane.normal);

        m_rightPlane.normal = glm::normalize(glm::cross(farRight - nearBottomRight, farRight - nearTopRight));
        m_rightPlane.dist = glm::dot(nearBottomRight, m_rightPlane.normal);

        m_topPlane.normal = glm::normalize(glm::cross(farTop - nearTopRight, farTop - nearTopLeft));
        m_topPlane.dist = glm::dot(nearTopLeft, m_topPlane.normal);

        m_bottomPlane.normal = glm::normalize(glm::cross(farBottom - nearBottomLeft, farBottom - nearBottomRight));
        m_bottomPlane.dist = glm::dot(nearBottomRight, m_bottomPlane.normal);


        glm::vec3 farTopLeft = farLeft + camUp * farHeight / 2.0f;
        glm::vec3 farTopRight = farRight + camUp * farHeight / 2.0f;
        glm::vec3 farBottomLeft = farTopLeft - camUp * farHeight;
        glm::vec3 farBottomRight = farTopRight - camUp * farHeight;
        
        voxr::DrawLine(nearTopLeft, farTopLeft);
        voxr::DrawLine(nearTopRight, farTopRight);
        voxr::DrawLine(nearBottomLeft, farBottomLeft);
        voxr::DrawLine(nearBottomRight, farBottomRight);
        
        voxr::DrawLine(farBottomLeft, farBottomRight);
        voxr::DrawLine(farTopLeft, farTopRight);
        voxr::DrawLine(farTopLeft, farBottomLeft);
        voxr::DrawLine(farTopRight, farBottomRight);

        voxr::DrawLine(camPos, camPos + camUp);
        voxr::DrawLine(camPos, camPos + camForward / 2.0f);
        voxr::DrawLine(camPos, camPos + camRight / 2.0f);
    }
}
