#pragma once
#include "IHCDescriptorWrap.h"

namespace IHCEngine::Graphics
{
	class IHCBuffer;

	class ComputeParticleDescriptorWrap : public IHCDescriptorWrap
	{
	public:
		ComputeParticleDescriptorWrap(IHCDevice& device);
		~ComputeParticleDescriptorWrap() override;
		void Setup() override;
		void CreateResources() override;
		void AllocAndBindDescriptorSets() override;

	private:
		const int PARTICLE_SYSTEM_COUNT = 10;
	};
}
