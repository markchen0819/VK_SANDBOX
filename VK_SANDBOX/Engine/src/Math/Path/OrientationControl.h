#pragma once
#include "SpaceCurve.h"

namespace IHCEngine::Math
{
	// Center of Interest - Foward mode

	struct OrientationControl
	{
		glm::vec3 globalUpVector = glm::vec3(0.0, 1.0, 0.0);

		// Returns in degrees
		glm::vec3 GetRotation(SpaceCurve& spaceCurve, float passedDistance)
		{
			// Close to end of the path, just look at tangent direction
			if (passedDistance >= 0.97) 
			{
				passedDistance = 0.97;
			}

			// Caculate forward position on the curve

			glm::vec3 lookAtPos =
					( spaceCurve.GetPositionOnCurve(passedDistance + 0.01)
					+ spaceCurve.GetPositionOnCurve(passedDistance + 0.02)
					+ spaceCurve.GetPositionOnCurve(passedDistance + 0.03)) / 3.0f;

			glm::vec3 W = normalize((lookAtPos - spaceCurve.GetPositionOnCurve(passedDistance)));
			glm::vec3 U = glm::cross(globalUpVector, W);
			glm::vec3 V = glm::cross(W, U);

			// Create a rotation matrix using the Frenet frame vectors (W, U, V)
			glm::mat4 rotationMatrix = glm::mat4(
				U.x, U.y, U.z, 0.0f,
				V.x, V.y, V.z, 0.0f, 
				W.x, W.y, W.z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f 
			);
			// Extract to angles (in degrees) to feed into Transform class
			glm::vec3 rotation = glm::degrees(glm::eulerAngles(glm::quat_cast(rotationMatrix)));

			return rotation;
		}
	};		

}
