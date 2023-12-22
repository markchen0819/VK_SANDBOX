#include "../../../pch.h"
#include "LineRendererComponent.h"

#include "../../Locator/GraphicsManagerLocator.h"
#include "../../../Graphics/RenderSystems/RenderSystem.h"
#include "../../../Graphics/VKWraps/IHCDevice.h"
#include "../../../Graphics/VKWraps/IHCSwapChain.h"
#include "../../../Graphics/VKWraps/IHCBuffer.h"

namespace IHCEngine::Component
{
	LineRendererComponent::LineRendererComponent()
	:Component(ComponentType::LineRenderer)
	{
		
	}

	LineRendererComponent::~LineRendererComponent()
	{
		auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
		vkDeviceWaitIdle(graphicsManager->GetIHCDevice()->GetDevice());
	}

	void LineRendererComponent::Draw(Graphics::FrameInfo& frameInfo)
	{
		if (points.size() <= 0) return;

		pointsBuffers[frameInfo.frameIndex]->WriteToBuffer((void*)points.data());
		pointsBuffers[frameInfo.frameIndex]->Flush();
		VkBuffer buffers[] = { pointsBuffers[frameInfo.frameIndex]->GetBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(frameInfo.commandBuffer, 0, 1, buffers, offsets);
		vkCmdDraw(frameInfo.commandBuffer, static_cast<uint32_t>(points.size()), 1, 0, 0);
	}

	void LineRendererComponent::SetPoints(std::vector<glm::vec3> points)
	{
		this->points.clear();
		for(auto& pt : points)
		{
			Vertex vertex;
			vertex.position = pt;
			// other attributes of vertex not used
			this->points.push_back(vertex);
		}
		allocateBuffer();
	}

	void LineRendererComponent::allocateBuffer()
	{
		// Allocate Buffer
		auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
		auto vertexCount = static_cast<uint32_t>(points.size());
		uint32_t vertexSize = sizeof(Vertex);

		vkDeviceWaitIdle(graphicsManager->GetIHCDevice()->GetDevice());
		pointsBuffers.resize(Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT; ++i)
		{
			pointsBuffers[i] = std::make_unique<Graphics::IHCBuffer>(
				*graphicsManager->GetIHCDevice(),
				vertexSize,
				vertexCount,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			pointsBuffers[i]->Map();
		}
	}

	void LineRendererComponent::Attach()
	{
		Core::GraphicsManagerLocator::GetGraphicsManager()->GetRenderSystem().AddGameObjectToRender(this->gameObject, Graphics::PipelineType::LINERENDERER);
	}

	void LineRendererComponent::Remove()
	{
		Core::GraphicsManagerLocator::GetGraphicsManager()->GetRenderSystem().RemoveGameObjectToRender(this->gameObject, Graphics::PipelineType::LINERENDERER);
	}
}
