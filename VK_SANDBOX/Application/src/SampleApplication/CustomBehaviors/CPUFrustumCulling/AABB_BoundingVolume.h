#pragma once
#include "Frustum.h"

struct AABB_BoundingVolume
{
    glm::vec3 center{ 0.0f, 0.0f, 0.0f };
    glm::vec3 halfWidth{ 0.0f, 0.f, 0.0f };

    AABB_BoundingVolume(const glm::vec3& center, float hx, float hy, float hz)
        : center(center), halfWidth(glm::vec3(hx, hy, hz))
    {

    }

    bool isOnOrOnTopOfPlane(const Plane& plane) const
    {
        // Projection interval radius
        // https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
        const float r = halfWidth.x * std::abs(plane.normal.x) + halfWidth.y * std::abs(plane.normal.y) + halfWidth.z * std::abs(plane.normal.z);
        auto signedDistanceToPlane = glm::dot(plane.normal, center) - plane.distance;
        return -r <= signedDistanceToPlane;
    }

    bool IsWithinFrustum(const Frustum& frustum) const
    {
        // Assume we don't change rotation or scale of the grassblock
        // skip rotation fix for AABB
        // https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling

        return (isOnOrOnTopOfPlane(frustum.leftFace) &&
            isOnOrOnTopOfPlane(frustum.rightFace) &&
            isOnOrOnTopOfPlane(frustum.topFace) &&
            isOnOrOnTopOfPlane(frustum.bottomFace) &&
            isOnOrOnTopOfPlane(frustum.nearFace) &&
            isOnOrOnTopOfPlane(frustum.farFace));
    };
};
