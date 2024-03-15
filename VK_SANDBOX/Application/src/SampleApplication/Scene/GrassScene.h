#pragma once
#include "../../Engine/src/Core/Scene/Scene.h"

namespace SampleApplication
{

	class GrassScene : public IHCEngine::Core::Scene
	{
	public:
		GrassScene();

		void Load() override;
		void UnLoad() override;

		void Init() override;
		void Reset() override;

		std::vector<IHCEngine::Core::GameObject*> GetGrassChunkGobjs(){ return grassChunkGobjs; }
	private:

		void createGridMeshAndLoadGridTexture();
		void createAxisMeshAndLoadAxisTexture();
		void createDirt();

		std::vector<IHCEngine::Core::GameObject*> grassChunkGobjs;

	};

}