#pragma once
#include "Component.h"
#include "../../../Graphics/VKWraps/VKHelpers.h"

namespace IHCEngine::Graphics
{
	class IHCBuffer;
}

namespace IHCEngine::Component
{
	class LineRendererComponent : public Component
	{
	public:

		LineRendererComponent();
		~LineRendererComponent();

		void Draw(Graphics::FrameInfo& frameInfo);
		void SetPoints(std::vector<glm::vec3> points);

	private:

		std::vector<Vertex> points;

		// vulkan
		std::vector<std::unique_ptr<Graphics::IHCBuffer>> pointsBuffers;

		void allocateBuffer();

		void Remove() override;
	};
}

