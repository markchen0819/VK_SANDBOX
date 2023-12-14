#pragma once

namespace IHCEngine::Graphics
{
	class IHCDevice;
	class IHCDescriptorManager;
	class IHCPipeline;
	struct FrameInfo;
}

namespace IHCEngine::Graphics
{
	class DefaultPipeline
	{

	public:
		DefaultPipeline(IHCDevice& device, VkRenderPass renderPass, const IHCDescriptorManager* descriptorManager);
		~DefaultPipeline();

		void Render(FrameInfo& frameInfo);

	private:

		IHCEngine::Graphics::IHCDevice& ihcDevice;
		const IHCDescriptorManager* descriptorManager;

		VkPipelineLayout pipelineLayout; // (CPU - shader stages, uniform buffers, samplers, and push constants)
		std::unique_ptr<IHCEngine::Graphics::IHCPipeline> pipeline;
		VkRenderPass renderPass;

		void createLayout();
		void createPipeline();
		void destroyPipeline();
	};

}