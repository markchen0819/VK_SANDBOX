#include "../../pch.h"
#include "InverseKinematicsSolver.h"

namespace IHCEngine::Graphics
{
	void InverseKinematicsSolver::SetJoints(const std::vector<glm::vec3>& initialJoints)
	{
		// from root to E.E.

		joints = initialJoints;
        totalDistance = 0;

		for (size_t i = 1; i < joints.size(); i++) 
		{
            float distance = glm::length(joints[i] - joints[i - 1]);
			distances.push_back(distance);
            totalDistance += distance;
		}
	}

	void InverseKinematicsSolver::Solve_FABRIK(glm::vec3 target)
	{
        // Reference
        // https://youtu.be/UNoX65PRehA?si=qRPyM7tonfIDzYfX

        // Cannot reach, too far away
        float distanceFromRootToTarget = glm::length(target - joints[0]);
        if(distanceFromRootToTarget > totalDistance)
        {
            glm::vec3 dir = glm::normalize(target - joints[0]);
            target = joints[0] + dir * distanceFromRootToTarget;
            // straight line
        }

        // Can reach
        int iteration = 0;
        float distanceFromEEtoTargetPos = glm::length(joints.back() - target);
        while (iteration < MAX_ITERATIONS && distanceFromEEtoTargetPos > USER_EPSILON)
        {
            // Backward reaching (E.E. to root)
            joints.back() = target;
            for (int i = joints.size() - 2; i >= 0; --i)
            {
                glm::vec3& pointA = joints[i];
                glm::vec3& pointB = joints[i+1];
                const float distanceAB = distances[i];
                glm::vec3 dir = glm::normalize(pointA - pointB);
                pointA = pointB + dir * distanceAB;
            }

            // Forward reaching  (root to E.E.)
            for (size_t i = 1; i < joints.size(); i++) 
            {
                glm::vec3& pointA = joints[i-1];
                glm::vec3& pointB = joints[i];
                const float distanceAB = distances[i-1];

                glm::vec3 dir = glm::normalize(pointB - pointA);
                pointB = pointA + dir * distanceAB;
            }

            // Keep getting closer target
            distanceFromEEtoTargetPos = glm::length(joints.back() - target);
            iteration++;
        }

	}
}
