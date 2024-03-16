#pragma once
#include "Component.h"
#include "../../../Graphics/Grass/GrassBlade.h"
#include "../../../Graphics/VKWraps/VKHelpers.h"
#include "../../../Graphics/Grass/ComputeGrassUniformBufferObject.h"

namespace IHCEngine::Graphics
{
	class IHCBuffer;
	class IHCTexture;
}


namespace IHCEngine::Component
{
	struct GrassBladePropertyOverride 
	{
		// Wind
		float windSpeed = 0.12f;
		glm::vec4 windDirection = glm::vec4(1.0, 0.01, 0, 0);
		float windStrength = 3.0f;
		float swayStrength = 0.05f;
		float swayFrequency = 5.0f;

		// Bezier curve
		int enableGlobalTilt = 0;
		float globalTilt = 75.0f;
		int enableControlPt = 0;
		float bend = 0.15f; 
		glm::vec4 controlPtA = glm::vec4(0.16f, 0.51f, 0.0f, 0.0f);
		glm::vec4 controlPtB = glm::vec4(0.53f, 0.93f, 0.0f, 0.0f);
	};

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

		// Noise Texture
		void SetNoiseTexture(Graphics::IHCTexture* texture);
		std::vector<VkDescriptorSet>& GetNoiseTextureDescriptorSet() const;
		Graphics::IHCTexture* GetNoiseTexture() const;
		void SetChunkCoords(int chunkX, int chunkY, int gridSizeX, int gridSizeY);

		// For debug, or artist define
		GrassBladePropertyOverride grassBladePropertyOverride;

	private:
		std::vector<Graphics::GrassBlade> grassBlades;
		float lastFrameTime = 0.0f;
		// Total area size, the world position ranges are chunk origin +- areaSize
		float areaSizeX = 20.0f;
		float areaSizeZ = 20.0f;
		// Split area into N x N Grid
		int maxGrassBladeCount = 6400; 
		int grassBladeCount = 6400;
		int dimensionX = 80; 
		int dimensionZ = 80;
		// Wind Texture
		Graphics::IHCTexture* noiseTexture = nullptr;

		void initGrassBlades();
		void updateGrassBladeProperties();

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
