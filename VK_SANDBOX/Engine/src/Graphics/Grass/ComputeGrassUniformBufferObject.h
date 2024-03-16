#pragma once
namespace IHCEngine::Graphics
{
	struct ComputeGrassUniformBufferObject
	{
		float deltaTime = 0.0f;
		float accumulatedTime = 0.0f;
		float windStrength = 3.0f;
		float windSpeed = 0.12f;
		int chunkX; // x coordinate of the chunk in the world grid
		int chunkY; // y coordinate of the chunk in the world grid
		int gridSizeX; // Total chunks along x dimension
		int gridSizeY; // Total chunks along y dimension
		float areaSize = 20.0f;
		float swayStrength = 0.05f;
		float swayFrequency = 5.0f;
		int enableGlobalTilt = 0; 
		float globalTilt = 75.0f;
		int enableControlPt = 0;
		float bend = 0.15f; // program calculated controlPtA and controlPtB
		float d3 = 0.0f;
		glm::vec4 windDirection = glm::vec4(1.0, 0.01, 0, 0);
		glm::vec4 controlPtA = glm::vec4(0.0f, 0.53f, 0.0f, 0.0f);
		glm::vec4 controlPtB = glm::vec4(0.0f, 0.53f, 0.0f, 0.0f);
	};
}
