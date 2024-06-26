#pragma once
#include "../../Engine/src/Core/Scene/Scene.h"

namespace SampleApplication
{

	class ParticleSystemScene : public IHCEngine::Core::Scene
	{
	public:
		ParticleSystemScene();

		void Load() override;
		void UnLoad() override;

		void Init() override;
		void Reset() override;

	private:

		void createGridMeshAndLoadGridTexture();
		void createAxisMeshAndLoadAxisTexture();

	};

}