#include "../../pch.h"
#include "Time.h"

std::unique_ptr<IHCEngine::Core::Time> timeInstance{ nullptr };
std::mutex IHCEngine::Core::Time::mutexLock;

void IHCEngine::Core::Time::Init()
{
	GetInstance();
}

IHCEngine::Core::Time* IHCEngine::Core::Time::GetInstance()
{
	std::lock_guard lock(mutexLock);
	if (timeInstance == nullptr)
	{
		timeInstance = std::make_unique<Time>();
	}
	return timeInstance.get();
}

IHCEngine::Core::Time::Time::Time()
{
	previousTime = std::chrono::high_resolution_clock::now();
}

void IHCEngine::Core::Time::Reset()
{
	timeInstance->frameCount = 0;
	timeInstance->totalFixedTime = 0;
	timeInstance->totalTime = 0;
	timeInstance->timeScale = 1.0f;
}

void IHCEngine::Core::Time::Update()
{
	const auto currentTime = std::chrono::high_resolution_clock::now();
	const float unscaledDeltaTime = static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()- timeInstance->previousTime).count()) * 0.000000001f;
	
	timeInstance->previousTime = currentTime;
	timeInstance->totalTime += unscaledDeltaTime;
	timeInstance->frameTimeCounter += unscaledDeltaTime;

	if (timeInstance->frameTimeCounter <= timeInstance->minFrameTime)
	{
		return; // framerate control
	}

	timeInstance->unscaledDeltaTime = timeInstance->frameTimeCounter;
	timeInstance->deltaTime = timeInstance->timeScale * timeInstance->frameTimeCounter;
	timeInstance->calculatedFps = 1.0f / timeInstance->frameTimeCounter;
	timeInstance->frameCount++;

	if (timeInstance->minFrameTime == 0.0f)
	{
		timeInstance->frameTimeCounter = 0.0f;
	}
	else
	{
		timeInstance->frameTimeCounter = timeInstance->frameTimeCounter - timeInstance->minFrameTime;
	}
}

void IHCEngine::Core::Time::UpdateFixedTime()
{
	timeInstance->totalFixedTime += timeInstance->unscaledFixedDeltaTime;
}
void IHCEngine::Core::Time::SetFixedTime(int maxUpdatePerFrame)
{
	timeInstance->unscaledFixedDeltaTime = 1.0f / static_cast<float>(maxUpdatePerFrame);
}
bool IHCEngine::Core::Time::ShouldExecuteFixedUpdate()
{
	// Should call fixed update when 
	// total fixed time is less than 1 fixed time step from total time
	return (timeInstance->totalTime - timeInstance->totalFixedTime > timeInstance->unscaledFixedDeltaTime);
}

