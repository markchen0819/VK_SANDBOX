#pragma once
namespace IHCEngine::Graphics
{
	struct ComputeParticleUniformBufferObject
	{
		float deltaTime = 0.0f;
		float accumulatedTime = 0.0f;
		int enableAdvection = 0;
		float flowMaxVelocity = 0.0f;

		float flowWidth = 10.0f;
		int enableVortex = 0;
		float kappa; // rotation rate 
		float tau; //  tightness

		int enableGravity = 0;
		int enableBounce = 0;
		float groundHeight = -4.8f;
		float restitution = 0.7f;

		int enableSpiral = 0;
		int spiralWithGlobalAxis = 0;
		float spiralRadius = 10.0f;
		float sprialAngularSpeed = 2;

		float sprialAxisSpeed = 5;
		int dummy1 = -1;
		int dummy2 = -1;
		int dummy3 = -1;

		glm::vec4 flowDirection = glm::vec4(0, 1, 0, 0);
		glm::vec4 flowCenter = glm::vec4(0, 1, 0, 0);
		glm::vec4 vortexCenter = glm::vec4(0, 0, 0, 0);
		glm::vec4 vortexAxis = glm::vec4(0, 1, 0, 0);
		glm::vec4 gravity = glm::vec4(0, -9.8, 0, 0);
	};
}
