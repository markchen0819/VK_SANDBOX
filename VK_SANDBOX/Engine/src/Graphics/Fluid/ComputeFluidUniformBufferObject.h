#pragma once
namespace IHCEngine::Graphics
{
	struct ComputeFluidUniformBufferObject
	{
		float deltaTime = 0.0f;
		float accumulatedTime = 0.0f;
		unsigned int particleCount = 256;
		float gasConstant = 0.0f;
		float restDensity = 0.0f;
		float viscosity = 0.0f;
	};
}
