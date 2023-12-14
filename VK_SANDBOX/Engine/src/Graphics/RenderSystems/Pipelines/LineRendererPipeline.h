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
	class LineRendererPipeline
	{

	public:
		LineRendererPipeline(IHCDevice& device, VkRenderPass renderPass, const IHCDescriptorManager* descriptorManager);
		~LineRendererPipeline();

		void Render(FrameInfo& frameInfo);

	private:

		IHCEngine::Graphics::IHCDevice& ihcDevice;
		const IHCDescriptorManager* descriptorManager;

		VkPipelineLayout pipelineLayout;
		std::unique_ptr<IHCEngine::Graphics::IHCPipeline> pipeline;
		VkRenderPass renderPass;

		void createLayout();
		void createPipeline();
		void destroyPipeline();
	};

}