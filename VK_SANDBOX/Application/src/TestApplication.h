#pragma once

#include "IApplication.h"

class TestApplication : IHCEngine::Core::IApplication
{
public:
	TestApplication();
	void Init() override;
	void Update() override;
	void Shutdown() override;

private:

};
