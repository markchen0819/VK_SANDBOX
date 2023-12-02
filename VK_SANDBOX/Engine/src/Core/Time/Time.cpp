#include "../../pch.h"
#include "Time.h"


void IHCEngine::Core::Time::Init()
{
	GetInstance();
}

IHCEngine::Core::Time::Time()
{
	startTime = std::chrono::high_resolution_clock::now();
	previousTime = std::chrono::high_resolution_clock::now();
}

void IHCEngine::Core::Time::Reset()
{
	instance->startTime = std::chrono::high_resolution_clock::now();
	instance->previousTime = std::chrono::high_resolution_clock::now();

	instance->frameTimeCounter = 0;

	instance->frameCount = 0;
	instance->timeScale = 1.0f;
	instance->totalTime = 0;
	instance->totalFixedTime = 0;
	instance->averageFps = 0.0;
	instance->shouldExecuteUpdate = false;
	instance->unscaledDeltaTime = 0.0f;
	instance->deltaTime = 0.0f;
}

void IHCEngine::Core::Time::Update()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	double elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - instance->startTime).count() / 1000.0;  // Convert to seconds as a double
	instance->unscaledDeltaTime = static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()- instance->previousTime).count()) * 0.000000001f;
	
	instance->previousTime = currentTime;
	instance->totalTime += instance->unscaledDeltaTime;
	instance->frameTimeCounter += instance->unscaledDeltaTime;

	if (instance->frameTimeCounter <= instance->minFrameTime) // lock frame rate
	{
		return; 
	}

	// window interrupt fix (not count up too much)
	if(instance->frameTimeCounter > instance->maxFrameTime)
	{
		instance->frameTimeCounter = instance->maxFrameTime;
	}

	instance->unscaledDeltaTime = instance->frameTimeCounter;
	instance->deltaTime = instance->timeScale * instance->frameTimeCounter;
	instance->calculatedFps = 1.0f / instance->frameTimeCounter;
	instance->shouldExecuteUpdate = true;
	instance->frameCount++;
	instance->averageFps = instance->frameCount / elapsedTime;

	if (instance->minFrameTime == 0.0f)
	{
		instance->frameTimeCounter = 0.0f;
	}
	else
	{
		instance->frameTimeCounter = instance->frameTimeCounter - instance->minFrameTime;
	}
}

bool IHCEngine::Core::Time::ShouldExecuteUpdate()
{
	if (instance->shouldExecuteUpdate)
	{
		instance->shouldExecuteUpdate = false;
		return true;
	}
	return false;
}

void IHCEngine::Core::Time::UpdateFixedTime()
{
	instance->totalFixedTime += instance->unscaledFixedDeltaTime;
}
void IHCEngine::Core::Time::SetFixedTime(int maxUpdatePerFrame)
{
	instance->unscaledFixedDeltaTime = 1.0f / static_cast<float>(maxUpdatePerFrame);
}
bool IHCEngine::Core::Time::ShouldExecuteFixedUpdate()
{
	// Should call fixed update when 
	// total fixed time is less than 1 fixed time step from total time
	return (instance->totalTime - instance->totalFixedTime > instance->unscaledFixedDeltaTime);
}

