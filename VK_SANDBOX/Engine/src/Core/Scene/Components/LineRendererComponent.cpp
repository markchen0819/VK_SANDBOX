#include "../../../pch.h"
#include "LineRendererComponent.h"

#include "../../Locator/GraphicsManagerLocator.h"
#include "../../../Graphics/VKWraps/IHCDevice.h"
#include "../../../Graphics/VKWraps/IHCSwapChain.h"
#include "../../../Graphics/VKWraps/IHCBuffer.h"

namespace IHCEngine::Component
{
	LineRendererComponent::LineRendererComponent()
	:Component(ComponentType::LineRenderer)
	{
		//test
		std::vector<glm::vec3> p;
		p.push_back(glm::vec3(1, 1, 0));
		p.push_back(glm::vec3(2, 1, 0));
		p.push_back(glm::vec3(3, 2, 0));
		p.push_back(glm::vec3(4, 2, 0));
		p.push_back(glm::vec3(5, 3, 0));
		p.push_back(glm::vec3(6, 3, 0));
		SetPoints(p);
	}

	void LineRendererComponent::Draw(Graphics::FrameInfo& frameInfo)
	{
		if (points.size() <= 0) return;

		pointsBuffers[frameInfo.frameIndex]->WriteToBuffer((void*)points.data());
		pointsBuffers[frameInfo.frameIndex]->Flush();
		VkBuffer buffers[] = { pointsBuffers[frameInfo.frameIndex]->GetBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(frameInfo.commandBuffer, 0, 1, buffers, offsets);
		vkCmdDraw(frameInfo.commandBuffer, points.size(), 1, 0, 0);
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

		//// Allocate Buffer
		//auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
		//std::vector<Vertex>& bonevertices = debugBoneVertices;
		//auto vertexCount = static_cast<uint32_t>(bonevertices.size());
		//uint32_t vertexSize = sizeof(bonevertices[0]);

		//vkDeviceWaitIdle(graphicsManager->GetIHCDevice()->GetDevice());

		//for (int i = 0; i < IHCSwapChain::MAX_FRAMES_IN_FLIGHT; ++i)
		//{
		//	debugBoneBuffers[i] = std::make_unique<Graphics::IHCBuffer>(
		//		*graphicsManager->GetIHCDevice(),
		//		vertexSize,
		//		vertexCount,
		//		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		//		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		//	);
		//	debugBoneBuffers[i]->Map();
		//}
	}

	void LineRendererComponent::Remove()
	{
	}
}
