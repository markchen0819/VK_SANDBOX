#include "../../pch.h"
#include "Time.h"


void IHCEngine::Core::Time::Init()
{
	GetInstance();
}

IHCEngine::Core::Time::Time()
{
	previousTime = std::chrono::high_resolution_clock::now();
}

void IHCEngine::Core::Time::Reset()
{
	frameCount = 0;
	totalFixedTime = 0;
	totalTime = 0;
	timeScale = 1.0f;
}

void IHCEngine::Core::Time::Update()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	unscaledDeltaTime = static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()- previousTime).count()) * 0.000000001f;
	
	previousTime = currentTime;
	totalTime += unscaledDeltaTime;
	frameTimeCounter += unscaledDeltaTime;

	if (frameTimeCounter <= minFrameTime)
	{
		return; // framerate control
	}

	unscaledDeltaTime = frameTimeCounter;
	deltaTime = timeScale * frameTimeCounter;
	calculatedFps = 1.0f / frameTimeCounter;
	frameCount++;

	if (minFrameTime == 0.0f)
	{
		frameTimeCounter = 0.0f;
	}
	else
	{
		frameTimeCounter = frameTimeCounter - minFrameTime;
	}
}

void IHCEngine::Core::Time::UpdateFixedTime()
{
	totalFixedTime += unscaledFixedDeltaTime;
}
void IHCEngine::Core::Time::SetFixedTime(int maxUpdatePerFrame)
{
	unscaledFixedDeltaTime = 1.0f / static_cast<float>(maxUpdatePerFrame);
}
bool IHCEngine::Core::Time::ShouldExecuteFixedUpdate()
{
	// Should call fixed update when 
	// total fixed time is less than 1 fixed time step from total time
	return (totalTime - totalFixedTime > unscaledFixedDeltaTime);
}

