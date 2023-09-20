#pragma once
#include "Component.h"

namespace IHCEngine::Component
{
	enum class PipelineType
	{
		DEFAULT,
		WIREFRAME,
		SKELETAL
	};

	class PipelineComponent : public Component
	{
	public:

		PipelineComponent();
		~PipelineComponent() = default;

		void SetPipelineType(PipelineType type) { pipelineType = type; };
		PipelineType GetPipelineType() { return pipelineType; }

	private:

		PipelineType pipelineType = PipelineType::DEFAULT;
		void Remove() override;
	};
}
