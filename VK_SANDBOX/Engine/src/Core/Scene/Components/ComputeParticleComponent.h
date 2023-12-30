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

		const int GetMaxParticleCount() { return maxParticleCount; }

		// Vulkan
		void SetDescriptorSets(std::vector<VkDescriptorSet> set) { computeDescriptorSets = set; }
		std::vector<VkDescriptorSet>& GetDescriptorSets() { return computeDescriptorSets; }

		void SetUniformBuffers(std::vector<Graphics::IHCBuffer*> buffers) { computeParticleUniformBuffers = buffers; }
		std::vector<Graphics::IHCBuffer*>& GetUnformBuffers() { return computeParticleUniformBuffers; }

		std::vector<Graphics::IHCBuffer*> GetSSBO()
		{
			std::vector<Graphics::IHCBuffer*> rawPointerVector;
			rawPointerVector.reserve(shaderStorageBuffers.size());
			for (const auto& uniquePtr : shaderStorageBuffers) 
			{
				rawPointerVector.push_back(uniquePtr.get());
			}
			return rawPointerVector;
		}

	private:

		std::vector<Graphics::Particle> particles;
		int maxParticleCount = 65536;// particle component contains how many particle

		void initParticles();

		// Vulkan
		std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>> shaderStorageBuffers; // created in component
		std::vector<Graphics::IHCBuffer*> computeParticleUniformBuffers; // retrieved from descriptor wrap
		std::vector<VkDescriptorSet> computeDescriptorSets; // retrieved from descriptor wrap
	
		void createVulkanResources();
		void createShaderStorageBuffers();
		void destroyVulkanResources();

		void Attach() override;
		void Remove() override;
	};
}
