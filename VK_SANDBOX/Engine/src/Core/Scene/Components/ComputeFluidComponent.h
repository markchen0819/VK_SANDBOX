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
		void SetRadius(const float f) { radius = f; }
		float GetRadius() const { return radius; }
		void SetMaxBound(const glm::vec4& vec) { maxBound = vec; }
		const glm::vec4& GetMaxBound() const { return maxBound; }
		void SetMinBound(const glm::vec4& vec) { minBound = vec; }
		const glm::vec4& GetMinBound() const { return minBound; }
		void SetInteractionSpherePos(const glm::vec4& vec) { interactionSpherePos = vec; }
		const glm::vec4& GetInteractionSpherePos() const { return interactionSpherePos; }

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
		std::vector<Graphics::IHCBuffer*> GetSpacialEntrySSBOs()
		{
			std::vector<Graphics::IHCBuffer*> rawPointerVector;
			rawPointerVector.reserve(spacialEntrySSBOs.size());
			for (const auto& uniquePtr : spacialEntrySSBOs)
			{
				rawPointerVector.push_back(uniquePtr.get());
			}
			return rawPointerVector;
		}
		std::vector<Graphics::IHCBuffer*> GetSpacialLookupSSBOs()
		{
			std::vector<Graphics::IHCBuffer*> rawPointerVector;
			rawPointerVector.reserve(spacialLookupSSBOs.size());
			for (const auto& uniquePtr : spacialLookupSSBOs)
			{
				rawPointerVector.push_back(uniquePtr.get());
			}
			return rawPointerVector;
		}


	private:
		std::vector<Graphics::FluidParticle> particles;
		std::vector<Graphics::SpacialEntry> spacialEntries;
		std::vector<Graphics::StartIndexForSpacialEntry> spacialLookups;

		int maxParticleCount = 4096;// 256 * 3;
		int particleCount = 4096;// 256 * 3;

		float gasConstant = 120.0f;// 16.0f;// 11.0;// 1.2f;
		float restDensity = 85.0f;// 100.0f;// 55.0f;
		float viscosity = 0.001f;
		float radius = 0.75f;// 1.0f;// 0.9;// 2.0f;
		glm::vec4 maxBound = glm::vec4(8.0f, 25.0f, 8.0f, -1.0f);
		glm::vec4 minBound = glm::vec4(-8.0f, -0.0f, -8.0f, -1.0f);
		glm::vec4 interactionSpherePos = glm::vec4(-99.0f, -99.0f, -99.0f, 0.0f);

		void initParticles();

		// Vulkan
		Graphics::ComputeFluidUniformBufferObject ubo{};
		std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>> shaderStorageBuffers; // created in component
		std::vector<Graphics::IHCBuffer*> computeParticleUniformBuffers; // retrieved from descriptor wrap
		std::vector<VkDescriptorSet> computeDescriptorSets; // retrieved from descriptor wrap

		std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>> spacialEntrySSBOs;
		std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>> spacialLookupSSBOs;


		void createVulkanResources();
		void createShaderStorageBuffers();
		void createSpacialSSBOs();
		void destroyVulkanResources();

		void Attach() override;
		void Remove() override;
	};
}
