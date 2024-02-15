#pragma once
#include "Component.h"
#include "../../../Graphics/Grass/GrassBlade.h"
#include "../../../Graphics/VKWraps/VKHelpers.h"
#include "../../../Graphics/Grass/ComputeGrassUniformBufferObject.h"

namespace IHCEngine::Graphics
{
	class IHCBuffer;
}

namespace IHCEngine::Component
{
	class ComputeGrassComponent : public Component
	{
	public:
		ComputeGrassComponent();
		~ComputeGrassComponent() override = default;

		int GetMaxGrassBladeCount() const { return maxGrassBladeCount; }
		int GetGrassBladeCount() const { return grassBladeCount; }

		void Compute(Graphics::FrameInfo& frameInfo);
		void Draw(Graphics::FrameInfo& frameInfo);

		// Vulkan
		void SetDescriptorSets(std::vector<VkDescriptorSet> set) { computeDescriptorSets = set; }
		std::vector<VkDescriptorSet>& GetDescriptorSets() { return computeDescriptorSets; }
		void SetUniformBuffers(std::vector<Graphics::IHCBuffer*> buffers) { computeGrassUniformBuffers = buffers; }
		std::vector<Graphics::IHCBuffer*>& GetUnformBuffers() { return computeGrassUniformBuffers; }
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
		std::vector<Graphics::GrassBlade> grassBlades;

		int maxGrassBladeCount = 3600; 
		int grassBladeCount = 3600;

		float lastFrameTime = 0.0f;

		void initGrassBlades();

		// Vulkan
		Graphics::ComputeGrassUniformBufferObject ubo{};
		std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>> shaderStorageBuffers; // created in component
		std::vector<Graphics::IHCBuffer*> computeGrassUniformBuffers; // retrieved from descriptor wrap
		std::vector<VkDescriptorSet> computeDescriptorSets; // retrieved from descriptor wrap

		void createVulkanResources();
		void createShaderStorageBuffers();
		void destroyVulkanResources();

		void Attach() override;
		void Remove() override;
	};
}
