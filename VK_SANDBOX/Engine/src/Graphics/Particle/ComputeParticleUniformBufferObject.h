#pragma once
namespace IHCEngine::Graphics
{

	struct ComputeParticleUniformBufferObject
	{
		float deltaTime = 1.0f;
		float accumulatedTime = 0.0f;
	};

}
