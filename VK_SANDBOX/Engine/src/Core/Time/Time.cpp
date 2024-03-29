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
	instance->frameTimeCounter = 0;

	instance->frameCount = 0;
	instance->timeScale = 1.0f;
	instance->totalTime = 0;
	instance->totalFixedTime = 0;
	instance->averageFps = 0.0;
	instance->shouldExecuteUpdate = false;
	instance->unscaledDeltaTime = 0.0f;
	instance->deltaTime = 0.0f;
	instance->elapsedDeltaTime = 0.0f;

	instance->startTime = std::chrono::high_resolution_clock::now();
	instance->previousTime = std::chrono::high_resolution_clock::now();
}

void IHCEngine::Core::Time::Update()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
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
	instance->elapsedDeltaTime += instance->deltaTime;
	instance->calculatedFps = 1.0f / instance->frameTimeCounter;
	instance->shouldExecuteUpdate = true;
	instance->frameCount++;
	PushFrameTime(instance->unscaledDeltaTime);
	CalculateAverageFrameRate();

	if (instance->minFrameTime == 0.0f)
	{
		instance->frameTimeCounter = 0.0f;
	}
	else
	{
		instance->frameTimeCounter = instance->frameTimeCounter - instance->minFrameTime;
	}
}

void IHCEngine::Core::Time::PushFrameTime(float deltaTime)
{
	instance->frameTimes.push(deltaTime);
	instance->frameTimesSum += deltaTime;

	// exceeded the size of our window, pop the oldest frame time
	if (instance->frameTimes.size() > instance->maxFrameTimesToConsider)
	{
		instance->frameTimesSum -= instance->frameTimes.front();
		instance->frameTimes.pop();
	}
}

void IHCEngine::Core::Time::CalculateAverageFrameRate() //Smoothed/ Moving Average FPS
{
	// Avoid division by zero
	if (instance->frameTimes.empty())
	{
		instance->averageFps = instance->calculatedFps;
		return;
	}

	// Average frame time
	float avgFrameTime = instance->frameTimesSum / instance->frameTimes.size();

	// Return FPS
	instance->averageFps = 1.0f / avgFrameTime;
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

