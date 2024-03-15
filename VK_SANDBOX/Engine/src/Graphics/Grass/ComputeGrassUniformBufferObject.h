#pragma once
namespace IHCEngine::Graphics
{
	struct ComputeGrassUniformBufferObject
	{
		float deltaTime = 0.0f;
		float accumulatedTime = 0.0f;
		float windStrength = 1.0f;
		float windSpeed = 0.1f;
		int chunkX; // x coordinate of the chunk in the world grid
		int chunkY; // y coordinate of the chunk in the world grid
		int gridSizeX; // Total chunks along x dimension
		int gridSizeY; // Total chunks along y dimension
		float areaSize = 20.0f;
		float dummy1 = 0.0f;
		float dummy2 = 0.0f;
		float dummy3 = 0.0f;
		glm::vec4 windDirection = glm::vec4(1.0, 0.01, 0, 0);
	};
}
