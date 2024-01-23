#pragma once
#include "Component.h"
#include "../../../Graphics/Particle/Particle.h"
#include "../../../Graphics/VKWraps/VKHelpers.h"

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
		const int GetParticleCount() { return particleCount; }

		void Compute(Graphics::FrameInfo& frameInfo);
		void Draw(Graphics::FrameInfo& frameInfo);

		// Operations
		void SetEnableAdvection(const bool b) { enableAdvection = b; }
		bool GetEnableAdvection() const { return enableAdvection; }
		void SetEnableVortex(const bool b) { enableVortex = b; }
		bool GetEnableVortex() const { return enableVortex; }

		void SetRotationRate(const float f) { kappa = f; }
		float GetRotationRate() const { return kappa; }
		void SetTightness(const float f) { tau = f; }
		float GetTightness() const { return tau; }

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

		int maxParticleCount = 65536 * 2; // 256;
		int particleCount = 65536 * 2; //256;
		float lastFrameTime = 0.0f;
		bool enableAdvection = false;
		bool enableVortex = false;
		float kappa = 2.0f;  // rotation rate 
		float tau = 0.5f; //  tightness

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
