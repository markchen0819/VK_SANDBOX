#pragma once
namespace IHCEngine::Graphics
{
	struct ComputeGrassUniformBufferObject
	{
		float deltaTime = 0.0f;
		float accumulatedTime = 0.0f;
		float windStrength = 3.0f;
		float windSpeed = 0.12f;

		float chunkX; // x coordinate of the chunk in the world grid
		float chunkY; // y coordinate of the chunk in the world grid
		float gridSizeX; // Total chunks along x dimension
		float gridSizeY; // Total chunks along y dimension

		float areaSize = 20.0f;
		float swayStrength = 0.1f;
		float swayFrequency = 7.0f;
		int enableGlobalTilt = 0;

		float globalTilt = 75.0f;
		int enableControlPt = 0;
		float bend = 0.15f; // program calculated controlPtA and controlPtB
		int enableRotationOverride = 0;

		int showWorldNormals = 0;
		int showLOD = 0;
		int isHighLOD = 0;
		int dummy = 0;

		glm::vec4 windDirection = glm::vec4(1.0, 0.01, 0, 0);
		glm::vec4 controlPtA = glm::vec4(0.0f, 0.53f, 0.0f, 0.0f);
		glm::vec4 controlPtB = glm::vec4(0.0f, 0.53f, 0.0f, 0.0f);
		glm::vec4 globalRotation = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec4 spherePosition = glm::vec4(1.0f, 5.0f, 0.0f, 1.0f);
	};
}
