#pragma once
#include "Component.h"
#include "../../../Graphics/Particle/Particle.h"

namespace IHCEngine::Graphics
{
	class IHCBuffer;
}

namespace IHCEngine::Component
{
	class ComputeParticleComponent : public Component
	{
	public:
		ComputeParticleComponent();
		~ComputeParticleComponent() = default;

		//// Vulkan
		//void SetDescriptorSets(std::vector<VkDescriptorSet> set) { descriptorSets = set; }
		//std::vector<VkDescriptorSet>& GetDescriptorSets() { return descriptorSets; }
		//void SetBuffers(std::vector<Graphics::IHCBuffer*> buffers) { computeParticleUniformBuffers = buffers; }
		//std::vector<Graphics::IHCBuffer*>& GetBuffers() { return computeParticleUniformBuffers; }


	private:

		std::vector<Graphics::Particle> particles;
		int PARTICLE_COUNT = 65536;// particle component contains how many particle
		void initParticles();

		// Vulkan
		std::vector<VkDescriptorSet> computeDescriptorSets;
		std::vector<VkDescriptorSet> descriptorSets;
		std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>> computeParticleUniformBuffers;
		std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>> shaderStorageBuffers;
		void createVulkanResources();
		void createShaderStorageBuffers();
		void createUniformBuffers();
		void createComputeDescriptorSets();

		void Attach() override;
		void Remove() override;
	};
}
