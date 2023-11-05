#pragma once
#include "../../Engine/src/Core/Scene/Scene.h"

namespace SampleApplication
{

	class InverseKinematicsScene : public IHCEngine::Core::Scene
	{
	public:
		InverseKinematicsScene();

		void Load() override;
		void UnLoad() override;

		void Init() override;
		void Reset() override;

	private:

		void createTargetObjectMesh();
		void createGridMeshAndLoadGridTexture();
		void createAxisMeshAndLoadAxisTexture();

	};

}