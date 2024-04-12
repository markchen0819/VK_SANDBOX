#pragma once
#include "Component.h"
#include "../../../Graphics/VKWraps/VKHelpers.h"
#include "../../../Graphics/Fluid/FluidParticle.h"
#include "../../../Graphics/Fluid/ComputeFluidUniformBufferObject.h"

namespace IHCEngine::Graphics
{
	class IHCBuffer;
}

namespace IHCEngine::Component
{
	class ComputeFluidComponent : public Component
	{
	public:
		ComputeFluidComponent();
		~ComputeFluidComponent() = default;

		const int GetMaxParticleCount() { return maxParticleCount; }
		const int GetParticleCount() { return particleCount; }

		void Compute(Graphics::FrameInfo& frameInfo);
		void Draw(Graphics::FrameInfo& frameInfo);

		// Parameters
		void SetGasConstant(const float f) { gasConstant = f; }
		float GetGasConstant() const { return gasConstant; }
		void SetRestDensity(const float f) { restDensity = f; }
		float GetRestDensity() const { return restDensity; }
		void SetViscosity(const float f) { viscosity = f; }
		float GetViscosity() const { return viscosity; }

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
		std::vector<Graphics::FluidParticle> particles;
		int maxParticleCount = 1000;// 256 * 3;
		int particleCount = 1000;// 256 * 3;

		float gasConstant = 0.75;//  2.75f;// 5.0;// 
		float restDensity = 15.0f; // 40.0f;// 10.0f;
		float viscosity = 0.005f;

		void initParticles();

		// Vulkan
		Graphics::ComputeFluidUniformBufferObject ubo{};
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
