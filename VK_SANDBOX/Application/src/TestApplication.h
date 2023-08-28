#pragma once
#include "IApplication.h"

namespace TestApplication
{
	class TestApplication : public IHCEngine::Core::IApplication
	{
	public:
		TestApplication(const std::string& appName);
		void Init() override;
		void Update() override;
		void Shutdown() override;

	private:
		int sceneNum;
	};
}
