#pragma once
namespace IHCEngine::Graphics
{
	struct ComputeGrassUniformBufferObject
	{
		float deltaTime = 0.0f;
		float accumulatedTime = 0.0f;
		float windStrength = 1.0f;
		float dummy = -1.0f;
		int chunkX; // x coordinate of the chunk in the grid
		int chunkY; // y coordinate of the chunk in the grid
		int gridSizeX; // Total chunks along x dimension
		int gridSizeY; // Total chunks along y dimension
		glm::vec4 windDirection = glm::vec4(1, -0.6, 0, 0);
	};
}
