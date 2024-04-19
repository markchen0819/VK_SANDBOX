#pragma once
namespace IHCEngine::Graphics
{
	struct ComputeFluidUniformBufferObject
	{
		glm::vec4 maxBound = glm::vec4(8.0f, 15.0f, 8.0f, -1.0f);
		glm::vec4 minBound = glm::vec4(-8.0f, -0.0f, -8.0f, -1.0f);
		glm::vec4 interactionSpherePos = glm::vec4(-99.0f, -99.0f, -99.0f, 1.0f);

		float deltaTime = 0.0f;
		float accumulatedTime = 0.0f;
		unsigned int particleCount = 256;
		float gasConstant = 0.0f;

		float restDensity = 0.0f;
		float viscosity = 0.0f;
		float radius = 2.0f;
		float interactionSphereRadius = 3.0f;
	};
}
