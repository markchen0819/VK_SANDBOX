#pragma once
#include <iostream>
#include <memory>

namespace IHCEngine::Core
{
	class IApplication
	{
	public:
		IApplication(const std::string& n) : name(n) {};
		~IApplication() = default;
		virtual void Init() = 0;
		virtual void Update() = 0;
		virtual void Shutdown() = 0;

		std::string GetName() { return name; }

	protected:
		std::string name="default Application";
	};

}
