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
	class DebugBonePipeline
	{

	public:
		DebugBonePipeline(IHCDevice& device, VkRenderPass renderPass, const IHCDescriptorManager* descriptorManager);
		~DebugBonePipeline();

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