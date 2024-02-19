#pragma once
namespace IHCEngine::Graphics
{
	struct ComputeGrassUniformBufferObject
	{
		float deltaTime = 0.0f;
		float accumulatedTime = 0.0f;
		float windStrength = 1.0f;
		float dummy = -1.0f;
		glm::vec4 windDirection = glm::vec4(1, -0.6, 0, 0);
	};
}
