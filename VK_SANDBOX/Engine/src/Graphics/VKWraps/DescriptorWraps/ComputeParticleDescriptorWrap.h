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

		// Get Resources to write into and store
		IHCBuffer* GetAvailableComputeParticleUBO();
		void ReleaseComputeParticleUBO(IHCBuffer* buffer);

	private:

		const int PARTICLE_SYSTEM_COUNT = 10;
		std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>> computeParticleUniformBuffers;
		std::stack<IHCBuffer*> availableComputeParticleUniformBuffers;

	};
}
