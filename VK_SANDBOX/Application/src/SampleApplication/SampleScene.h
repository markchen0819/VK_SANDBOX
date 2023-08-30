#pragma once
#include "../../Engine/src/Core/Scene/Scene.h"

namespace SampleApplication
{

	class SampleScene : public IHCEngine::Core::Scene
	{
	public:
		SampleScene();

		void Load() override;
		void UnLoad() override;

		void Init() override;
		void Reset() override;
	};

}