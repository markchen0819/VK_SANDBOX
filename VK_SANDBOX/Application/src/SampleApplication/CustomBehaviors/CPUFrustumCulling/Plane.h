#pragma once

struct Plane
{
    // Ax + By + Cz + D = 0
    glm::vec3 normal;
    float distance; // Distance with origin of world

    Plane(): normal(glm::vec3(0,0,0)), distance(-1.0f)
    {
	    
    }
    Plane(const glm::vec3& pt, const glm::vec3& norm) : normal(glm::normalize(norm)), distance(glm::dot(normal, pt))
    {

    }
};