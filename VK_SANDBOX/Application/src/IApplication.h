#pragma once
#include <memory>

namespace IHCEngine::Core
{
	class IApplication
	{
	public:
		virtual ~IApplication() = default;
		virtual void Init() = 0;
		virtual void Update() = 0;
		virtual void Shutdown() = 0;
	};
}
