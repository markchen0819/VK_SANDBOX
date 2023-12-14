#pragma once
#include "../../Engine/src/Core/Scene/Scene.h"

namespace SampleApplication
{
	class PhysicallyBasedSimulationScene : public IHCEngine::Core::Scene
	{
	public:
		PhysicallyBasedSimulationScene();

		void Load() override;
		void UnLoad() override;

		void Init() override;
		void Reset() override;

	private:
		void createClothMeshAndLoadClothTexture();
		void createGridMeshAndLoadGridTexture();
		void createAxisMeshAndLoadAxisTexture();

	};

}