#pragma once
#include "../../Engine/src/Core/Scene/Scene.h"

namespace SampleApplication
{

	class MotionAlongPathScene : public IHCEngine::Core::Scene
	{
	public:
		MotionAlongPathScene();

		void Load() override;
		void UnLoad() override;

		void Init() override;
		void Reset() override;

	private:

		void createControlPointMesh();
		void createGridMeshAndLoadGridTexture();
		void createAxisMeshAndLoadAxisTexture();

	};

}