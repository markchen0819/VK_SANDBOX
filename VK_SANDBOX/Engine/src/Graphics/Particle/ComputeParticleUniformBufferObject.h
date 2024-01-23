#pragma once
namespace IHCEngine::Graphics
{
	struct ComputeParticleUniformBufferObject
	{
		float deltaTime = 0.0f; // 4 bytes
		float accumulatedTime = 0.0f; // 4 bytes
		int enableAdvection = 0; // 4 bytes, but aligned to 16 bytes
		int enablVortex = 0; // 4 bytes
		float kappa; // 4 bytes  // rotation rate 
		float tau; // 4 bytes  //  tightness
	};
}
