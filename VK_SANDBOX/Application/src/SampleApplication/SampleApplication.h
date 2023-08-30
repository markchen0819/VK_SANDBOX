#pragma once
#include "../IApplication.h"

namespace SampleApplication
{
	class SampleApplication : public IHCEngine::Core::IApplication
	{
	public:
		SampleApplication(const std::string& appName);
		void Init() override;
		void Update() override;
		void Shutdown() override;

	private:
		int sceneNum;
	};
}
